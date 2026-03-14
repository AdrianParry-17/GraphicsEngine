#pragma once

#include "Engine_Constant.h"

namespace Engine {
    /**
     * @brief Represents a one-dimensional numeric range [start, end].
     */
    struct Range {
        /**
         * @brief The start of the range.
         */
        double start = 0;
        /**
         * @brief The end of the range.
         */
        double end = 1;
        
        /** @brief Default constructor. Initializes to [0, 1]. */
        Range() = default;
        /**
         * @brief Constructor.
         * @param _start The start of the range.
         * @param _end The end of the range.
         */
        Range(double _start, double _end) : start(_start), end(_end) {}

        bool operator==(const Range& r) const { return start == r.start && end == r.end; }
        bool operator!=(const Range& r) const { return !operator==(r); }
    
        /**
         * @brief Checks whether this range is identical to another within a small tolerance.
         * @param r The range to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(const Range& r) const {
            return NumericConstants::IsNearZero(start - r.start) && NumericConstants::IsNearZero(end - r.end);
        }

        /**
         * @brief Maps a value x from this range to the target range r.
         * @param r The target range.
         * @param x The value within this range to map.
         * @return The mapped value in range r.
         */
        double MapTo(const Range& r, double x) const {
            double denom = end - start;
            if (NumericConstants::IsNearZero(denom))
                denom = (denom < 0 ? -1 : 1) * NumericConstants::NearZero;
            return (x * (r.end - r.start) + (end * r.start - r.end * start)) / denom;
        }
    };
    
    /**
     * @brief Maps values from one Range to another.
     */
    struct RangeMapper {
        /**
         * @brief The input range.
         */
        Range input;
        /**
         * @brief The output range.
         */
        Range output;

        /** @brief Default constructor. */
        RangeMapper() = default;
        /**
         * @brief Constructor from two Range objects.
         * @param _i The input range.
         * @param _o The output range.
         */
        RangeMapper(const Range& _i, const Range& _o) : input(_i), output(_o) {}
        /**
         * @brief Constructor from individual range bounds.
         * @param _start_i Start of the input range.
         * @param _end_i End of the input range.
         * @param _start_o Start of the output range.
         * @param _end_o End of the output range.
         */
        RangeMapper(double _start_i, double _end_i, double _start_o, double _end_o) : input(_start_i, _end_i), output(_start_o, _end_o) {}

        bool operator==(const RangeMapper& r) const { return input == r.input && output == r.output; }
        bool operator!=(const RangeMapper& r) const { return !operator==(r); }
    
        /**
         * @brief Checks whether this mapper is identical to another within a small tolerance.
         * @param r The mapper to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(const RangeMapper& r) const { return input.IsIdentical(r.input) && output.IsIdentical(r.output); }
    
        /**
         * @brief Maps a value from the input range to the output range.
         * @param x The input value.
         * @return The mapped output value.
         */
        double Map(double x) const { return input.MapTo(output, x); }
    };
}