import unittest

from tools.gen import dbc
from tools.gen.errors import Diagnostics
from tools.gen.patterns.codec import bit_positions


class DbcImporter(unittest.TestCase):
    def test_parses_messages_and_signals(self):
        text = ('VERSION ""\n\nNS_ :\n    CM_\n    BA_\n\nBS_:\n\nBU_: A B\n\n'
                'BO_ 291 Speed: 8 A\n SG_ VehicleSpeed : 0|16@1+ (0.01,0) [0|300] "km/h" B\n'
                ' SG_ Accel : 23|12@0- (0.001,-1) [-2|2] "g" B\n\nCM_ SG_ 291 VehicleSpeed "speed";\n')
        d = Diagnostics()
        msgs = dbc.parse(text, "x.dbc", d)
        self.assertTrue(d.ok(), d.sorted())
        self.assertEqual(len(msgs), 1)
        m = msgs[0]
        self.assertEqual((m["name"], m["can_id"], m["payload_length"]), ("Speed", 291, 8))
        s0, s1 = m["signals"]
        self.assertEqual((s0["name"], s0["byte_order"], s0["signed"], s0["factor"]), ("vehicle_speed", "little_endian", False, 0.01))
        self.assertEqual((s1["name"], s1["byte_order"], s1["signed"], s1["offset"]), ("accel", "big_endian", True, -1))

    def test_rejects_multiplexing_and_float_types(self):
        for line in (' SG_ Mode M : 0|8@1+ (1,0) [0|255] "" B', 'SIG_VALTYPE_ 291 X : 1;'):
            with self.subTest(line=line):
                d = Diagnostics()
                dbc.parse('BO_ 1 M: 8 A\n SG_ X : 0|8@1+ (1,0) [0|255] "" B\n' + line + "\n", "x.dbc", d)
                self.assertIn("GEN-037", d.rules())

    def test_snake_case(self):
        self.assertEqual(dbc.snake("E2eErrorCount"), "e2e_error_count")
        self.assertEqual(dbc.snake("NearestDistance"), "nearest_distance")
        self.assertEqual(dbc.snake("ABSActive"), "abs_active")


class BitWalk(unittest.TestCase):
    def test_matches_runtime_motorola_walk(self):
        # Same expectations as tests/runtime/bits_test.cpp (MotorolaDbcSawtooth / MotorolaCrossesThreeBytes).
        self.assertEqual(bit_positions(15, 16, True), list(range(15, 7, -1)) + list(range(23, 15, -1)))
        self.assertEqual(bit_positions(3, 20, True)[:5], [3, 2, 1, 0, 15])
        self.assertEqual(bit_positions(4, 12, False), list(range(4, 16)))


if __name__ == "__main__":
    unittest.main()
