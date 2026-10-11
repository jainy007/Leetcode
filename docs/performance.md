# Performance

Benchmarks: `bench/` (Google Benchmark). Run with:
```bash
cmake --preset linux-host-bench && cmake --build --preset linux-host-bench --target ea_bench
./build/linux-host-bench/bench/ea_bench --benchmark_repetitions=3 --benchmark_report_aggregates_only=true
```

## Optimization rules
1. **Measure first.** Every optimization starts from a benchmark in `bench/` and is re-measured afterwards.
2. **Bit-identical results.** An optimization must not change any output bit. SIL results and
   trace evidence stay valid only because outputs are identical. Each optimized routine has a test
   that compares it exactly with its reference implementation.
3. **Prove at compile time.** Checks the generator can prove become `static_assert`s instead of
   runtime branches. Example: signal positions in generated codecs.
4. **No change to the safety contract.** Range checks, E2E checks, deadline and plausibility checks
   stay. Only their cost goes down.

## Results (Release -O3, x86-64 Xeon @ 2.1 GHz, shared VM, median of 3; noise ~ +/-15 %)

| Benchmark | Before | After | Speed-up | Change |
|---|---|---|---|---|
| Gate cycle, valid frame (SOME/IP + E2E + decode + debounce + SM + status) | 500 ns | 48 ns | 10x | all below |
| Gate cycle, no frame | 121 ns | 32 ns | 3.8x | status encode |
| `decode_object_summary` (4 signals) | 109 ns | 4.3 ns | 25x | compile-time positions, no `Result` per signal |
| `encode_gate_status` (5 signals) | 119 ns | 26 ns | 4.5x | compile-time positions; inline exact rounding |
| `extract` big endian 32-bit unaligned (runtime API) | 81 ns | 14 ns | 5.9x | byte-chunk algorithm instead of bit loop |
| CRC32-P4, 4 KiB | 11.8 us | 2.15 us | 5.5x | slicing-by-8 |
| CRC32-P4, 18 B | 25 ns | 4.8 ns | 5.2x | slicing-by-8 + 4-byte tail step |
| E2E P4 protect + check, 18 B | 40 ns | 33 ns | 1.2x | 4-byte tail step (CRC spans two short pieces) |
| `cpu_ref` dense 1024x256 | 295 us | 37 us | 7.9x | loop interchange (rows contiguous, vectorizable) |
| `cpu_ref` dense 256x64 | 17.3 us | 3.0 us | 5.9x | loop interchange |

### Equivalence evidence
- `Bits.ExhaustiveEquivalenceWithReference`: every start bit 0..81 and length 0..65, both byte
  orders, on a 10-byte payload. Compares `signal_fits`, `extract` and `insert` with the original
  bit-by-bit algorithm, including that neighbouring bits stay unchanged.
- `Crc.SlicingBy8MatchesBitwiseForAllLengthsAndSplits`: all lengths 0..300 for both polynomials,
  one-shot and streaming.
- `Rounding.MatchesLlroundIncludingTies`: ties, values next to 0.5, and 2 million random values,
  all compared with `std::llround`.
- `CpuRefDense.BitIdenticalToReferenceOrder`: exact equality (`EXPECT_EQ` on floats) with the
  textbook loop order.

## Not optimized (yet), with reasons
- `ObstacleScoreStage::run` (73 ns): dominated by two monotonic clock reads, which the deadline
  check (GPU-009) needs, and the virtual backend calls. The model is tiny; real models are bound
  by the accelerator.
- Seqlock and SPSC ring: already a few ns, limited by memory ordering.
