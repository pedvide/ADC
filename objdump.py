"""Run objdump on the .elf file to get the assembly for inspection. Only runs in debug mode"""

Import("env")

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(
        "arm-none-eabi-objdump -d -S -s -Mforce-thumb $BUILD_DIR/${PROGNAME}.elf > $BUILD_DIR/${PROGNAME}.s",
        "Building ${PROGNAME}.s with objdump"
    )
)
    
