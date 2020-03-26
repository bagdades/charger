##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.6.0] date: [Fri Jan 24 20:02:57 EET 2020] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = charger


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Os


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
src/main.c \
src/lcd_8544.c

# ASM sources
ASM_SOURCES =  \


#######################################
# binaries
#######################################
AS = avr-gcc
CC = avr-gcc
LD = avr-gcc
CP = avr-objcopy
OD = avr-objdump
SZ = avr-size
RM = rm
# HEX = $(CP) -O ihex
# BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
#Mcu
#------------------------------------------------------------------------------
MCU = mcu=atmega16


# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS += F_CPU=8000000UL


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
inc


# compile gcc flags
ASFLAGS = -m$(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -std=gnu99   					#-ansi -std=c99 -std=gnu99
# CFLAGS += $(addprefix -m, $(MCU))
CFLAGS += $(addprefix -D, $(C_DEFS))
CFLAGS += $(addprefix -I, $(C_INCLUDES))
CFLAGS += $(addprefix -m, $(MCU))

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
# CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script

# libraries
LIBS = 
# LIBS = -lc -lm -lnosys 
LIBDIR = 
# LDFLAGS = -m$(MCU) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map, -Wl,--gc-sections
LDFLAGS += -gc-section
LDFLAGS += $(addprefix -m, $(MCU))

#Secondary outputs
#------------------------------------------------------------------------------
LSS += $(BUILD_DIR)/$(TARGET).lss
EEPROM_IMAGE += $(BUILD_DIR)/$(TARGET).eep
SIZEDUMMY += sizedummy

# default action: build all
all: $(BUILD_DIR)/$(TARGET).hex secondary-outputs


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	$(CC)  $(CFLAGS) -MD -c $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) $(AFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@  $(OBJECTS)

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(CP) -Oihex $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/$(TARGET).lss: $(BUILD_DIR)/$(TARGET).elf
	$(OD) -h -S $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/$(TARGET).lss	

$(BUILD_DIR)/$(TARGET).eep: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -Oihex $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).eep

sizedummy: $(BUILD_DIR)/$(TARGET).elf 
	$(SZ) --format=avr   --$(MCU) $(BUILD_DIR)/$(TARGET).elf

secondary-outputs: $(LSS) $(EEPROM_IMAGE) $(SIZEDUMMY)
	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
