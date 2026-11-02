namespace simdutf {
namespace SIMDUTF_IMPLEMENTATION {
namespace {
namespace util {

simdutf_really_inline const char *find(const char *start, const char *end,
                                       char character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;
  constexpr size_t width = sizeof(simd8<uint8_t>);
  if (size_t(end - start) >= width) {
    const simd8<uint8_t> needle = simd8<uint8_t>(uint8_t(character));
    const uint8_t *p = reinterpret_cast<const uint8_t *>(start);
    const uint8_t *const e = reinterpret_cast<const uint8_t *>(end);
    // Check the first register where the input starts, then continue from
    // the next aligned address: the bytes before it have been checked.
    uint64_t matches = uint32_t((simd8<uint8_t>(p) == needle).to_bitmask());
    if (matches != 0) {
      return start + trailing_zeroes(matches);
    }
    p += width - reinterpret_cast<uintptr_t>(p) % width;
    // 64 bytes at a time, with one test per block.
    for (; size_t(e - p) >= 64; p += 64) {
      simd8x64<uint8_t> input(p);
      simd8<bool> any = input.chunks[0] == needle;
      for (int i = 1; i < simd8x64<uint8_t>::NUM_CHUNKS; i++) {
        any = any | (input.chunks[i] == needle);
      }
      if (any.to_bitmask() != 0) {
        return reinterpret_cast<const char *>(p) +
               trailing_zeroes(input.eq(uint8_t(character)));
      }
    }
    // One register at a time. The last one ends at the end of the input, so
    // it may start before p: shift out the bits of the bytes before p.
    for (; p < e; p += width) {
      const uint8_t *q = size_t(e - p) >= width ? p : e - width;
      matches = uint32_t((simd8<uint8_t>(q) == needle).to_bitmask()) >> (p - q);
      if (matches != 0) {
        return reinterpret_cast<const char *>(p) + trailing_zeroes(matches);
      }
    }
    return end;
  }
  // Inputs shorter than one register
  const size_t len = size_t(end - start);
  for (size_t i = 0; i < len; i++) {
    if (start[i] == character) {
      return start + i;
    }
  }
  return end;
}

simdutf_really_inline const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;
  constexpr size_t width = sizeof(simd16<uint16_t>) / sizeof(uint16_t);
  if (size_t(end - start) >= width) {
    const simd16<uint16_t> needle = simd16<uint16_t>(uint16_t(character));
    const uint16_t *p = reinterpret_cast<const uint16_t *>(start);
    const uint16_t *const e = reinterpret_cast<const uint16_t *>(end);
    // The bitmasks have two bits per code unit.
    uint64_t matches =
        uint32_t((simd16<uint16_t>::load(p) == needle).to_bitmask());
    if (matches != 0) {
      return start + trailing_zeroes(matches) / 2;
    }
    // Continue from the next aligned address if the input is aligned on two
    // bytes, otherwise from the end of the first register.
    const uintptr_t misalignment =
        reinterpret_cast<uintptr_t>(p) % sizeof(simd16<uint16_t>);
    p += misalignment % 2 == 0 ? width - misalignment / 2 : width;
    for (; size_t(e - p) >= 32; p += 32) {
      simd16x32<uint16_t> input(p);
      simd16<bool> any = input.chunks[0] == needle;
      for (int i = 1; i < simd16x32<uint16_t>::NUM_CHUNKS; i++) {
        any = any | (input.chunks[i] == needle);
      }
      if (any.to_bitmask() != 0) {
        return reinterpret_cast<const char16_t *>(p) +
               trailing_zeroes(input.eq(uint16_t(character))) / 2;
      }
    }
    for (; p < e; p += width) {
      const uint16_t *q = size_t(e - p) >= width ? p : e - width;
      matches = uint32_t((simd16<uint16_t>::load(q) == needle).to_bitmask()) >>
                (2 * (p - q));
      if (matches != 0) {
        return reinterpret_cast<const char16_t *>(p) +
               trailing_zeroes(matches) / 2;
      }
    }
    return end;
  }
  // Inputs shorter than one register
  const size_t len = size_t(end - start);
  for (size_t i = 0; i < len; i++) {
    if (start[i] == character) {
      return start + i;
    }
  }
  return end;
}

} // namespace util
} // namespace
} // namespace SIMDUTF_IMPLEMENTATION
} // namespace simdutf
