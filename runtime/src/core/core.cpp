#include <cstdio>
#include <cstdlib>

#include "ea/core/assert.hpp"
#include "ea/core/result.hpp"

namespace ea {

void panic(const char* file, int line, const char* expr) noexcept {
    (void)std::fprintf(stderr, "EA_ASSERT failed: %s (%s:%d)\n", expr, file, line);
    std::abort();
}

const char* to_string(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::kOk:
            return "Ok";
        case ErrorCode::kInvalidArgument:
            return "InvalidArgument";
        case ErrorCode::kOutOfRange:
            return "OutOfRange";
        case ErrorCode::kBufferTooSmall:
            return "BufferTooSmall";
        case ErrorCode::kLengthMismatch:
            return "LengthMismatch";
        case ErrorCode::kCrcMismatch:
            return "CrcMismatch";
        case ErrorCode::kSequenceError:
            return "SequenceError";
        case ErrorCode::kProtocolError:
            return "ProtocolError";
        case ErrorCode::kTimeout:
            return "Timeout";
        case ErrorCode::kDeadlineMiss:
            return "DeadlineMiss";
        case ErrorCode::kResourceExhausted:
            return "ResourceExhausted";
        case ErrorCode::kNotInitialized:
            return "NotInitialized";
        case ErrorCode::kAlreadyInitialized:
            return "AlreadyInitialized";
        case ErrorCode::kNotSupported:
            return "NotSupported";
        case ErrorCode::kOsError:
            return "OsError";
        case ErrorCode::kIoError:
            return "IoError";
        case ErrorCode::kIntegrityError:
            return "IntegrityError";
        case ErrorCode::kPlausibilityError:
            return "PlausibilityError";
    }
    return "Unknown";
}

}  // namespace ea
