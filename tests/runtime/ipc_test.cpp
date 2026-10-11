#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <thread>

#include "ea/ipc/seqlock.hpp"
#include "ea/ipc/shm_channel.hpp"
#include "ea/ipc/spsc_ring.hpp"

namespace {

struct Sample {
    std::uint64_t seq;
    std::uint64_t check;  // == seq * 3, detects torn reads
    double value;
};

TEST(SpscRing, FifoAndFull) {
    ea::ipc::SpscRing<int, 4> r;
    for (int i = 0; i < 4; ++i) {
        EXPECT_TRUE(r.push(i));
    }
    EXPECT_FALSE(r.push(99));
    int v = -1;
    for (int i = 0; i < 4; ++i) {
        ASSERT_TRUE(r.pop(v));
        EXPECT_EQ(v, i);
    }
    EXPECT_FALSE(r.pop(v));
}

TEST(SpscRing, ConcurrentProducerConsumer) {
    static ea::ipc::SpscRing<std::uint32_t, 64> ring;
    constexpr std::uint32_t kCount = 200000U;
    std::thread producer([] {
        for (std::uint32_t i = 0U; i < kCount;) {
            if (ring.push(i)) {
                ++i;
            }
        }
    });
    std::uint32_t expected = 0U;
    while (expected < kCount) {
        std::uint32_t v = 0U;
        if (ring.pop(v)) {
            ASSERT_EQ(v, expected);
            ++expected;
        }
    }
    producer.join();
}

TEST(Seqlock, NeverTears) {
    static ea::ipc::Seqlock<Sample> lock;
    EXPECT_EQ(lock.load().error(), ea::ErrorCode::kNotInitialized);
    std::atomic<bool> done{false};
    std::thread writer([&done] {
        for (std::uint64_t i = 1U; i <= 100000U; ++i) {
            lock.store(Sample{i, i * 3U, static_cast<double>(i)});
        }
        done.store(true);
    });
    std::uint64_t reads = 0U;
    while (!done.load()) {
        const auto s = lock.load();
        if (s.ok()) {
            ASSERT_EQ(s->check, s->seq * 3U);
            ++reads;
        }
    }
    writer.join();
    EXPECT_EQ(lock.version(), 100000U);
    EXPECT_GT(reads, 0U);
}

TEST(ShmChannel, CrossProcess) {
    char name[64];
    (void)std::snprintf(name, sizeof(name), "/ea_test_chan_%d", static_cast<int>(getpid()));
    auto pub = ea::ipc::ShmPublisher<Sample>::create(name);
    ASSERT_TRUE(pub.ok());
    pub->publish(Sample{1U, 3U, 1.0});

    const pid_t child = fork();
    ASSERT_GE(child, 0);
    if (child == 0) {
        auto sub = ea::ipc::ShmSubscriber<Sample>::open(name);
        if (!sub.ok()) {
            _exit(1);
        }
        const auto s = sub->latest();
        _exit((s.ok() && s->seq == 1U && s->check == 3U) ? 0 : 2);
    }
    int status = 0;
    ASSERT_EQ(waitpid(child, &status, 0), child);
    ASSERT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
}

TEST(ShmChannel, RejectsSizeMismatch) {
    char name[64];
    (void)std::snprintf(name, sizeof(name), "/ea_test_mis_%d", static_cast<int>(getpid()));
    auto pub = ea::ipc::ShmPublisher<Sample>::create(name);
    ASSERT_TRUE(pub.ok());
    EXPECT_FALSE(ea::ipc::ShmSubscriber<std::uint64_t>::open(name).ok());
    EXPECT_FALSE(ea::ipc::ShmPublisher<Sample>::create(name).ok());  // O_EXCL
}

}  // namespace
