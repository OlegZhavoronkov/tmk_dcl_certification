function(enable_warnings TARGET)
    target_compile_options(${TARGET} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror -Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wno-unused-function>
        # Disable maybe-uninitialized warning for GCC due to false positives
        $<$<CXX_COMPILER_ID:GNU>:
        -Wno-maybe-uninitialized>)
endfunction()
