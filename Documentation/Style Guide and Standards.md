#About This Guide

This guide is an attempt to have a single answer as to how this project should be formatted. It covers a large range of stylistic decisions, ranging from general file layout to variable naming. Both code and code documentation are covered. General documentation (such as markdown files) is not specifically covered, but may use this guide as a rough outline.

Since its inception, this guide has also been expanded such that it covers coding standards.

Note that this guide is not intended to be the answer to everything in life; it is purely to keep these files consistent. If you disagree with these points, feel free to ignore them for your own projects.


#General Notes

This style guide should define points as "should be" or similar. Absolutes shouldn't be in here because I'll probably make mistakes.

---

Tabs are to be used for indentation and spaces for alignment. There are no excuses.

+ Changing the tab width should not break anything under any circumstances.
+ Tabs and spaces are to be mixed if called for in the situation.

---

All files should contain a trailing newline character.

---

An effort should be made to keep all lines under about 100 characters long.

+ This assumes a tab width of 2.
+ Lines should be broken on punctuation, if possible.
+ If the intuitive understanding of the line is inhibited by this, it should not be split.
+ Lines within deeply nested structures should not be split, though it is likely such code would benefit from being restructured.

---

Code should be visually broken into logical sections if practical.

+ These sections should be separated by a single empty line.
+ Sections may be preceded by an inline comment as a title when needed.
+ Larger code sections should be wrapped in curly braces when practical.


#Code Hierarchy Terminology

Code files can be classified as "main project files", "main library files", or "modules".
The terms are distinguished by the hierarchy of files and their functionality.

+ Modules are files that are co-dependent with other files, either higher or lower in hierarchy.
	+ They contain functions and constants relevant to a general topic.
	+ They are intended to be specific to the project or library they are a part of.
	+ They should be located within a subdirectory of the containing project or library.
+ A module or group of modules that are not dependent on files outside the group is a library.
	+ Libraries should deal with some general topic.
	+ A single file within the library should be considered the main library file.
		+ This file should not be within a subdirectory of the library.
		+ This file should contain all functions and constants files outside the library depend on.
		+ The header for the main library file should be documented such to represent the library as a whole.
	+ They are capable of being (and are intended to be) shared across multiple projects.
	+ They should be either located within a subdirectory of the project or in a system-wide shared library directory.
+ The highest level file in a project is the main project file.
	+ This file should be capable of being run directly.
	+ No other file should be exclusively dependent on this file. Co-dependence is acceptable.

---

+ Every project should have exactly one main project file.
+ Projects may include one or more libraries and/or modules.
+ Every library contains one or more modules.
	+ Every library should contain exactly one main library file.
		+ This file represents the library as a whole.
		+ This file should contain all functions and constants that may be referenced by files above it in hierarchy.
	+ A single module may be a library (and therefore the main library file) if it does not depend upon any files higher in hierarchy.


#Header Files

All source files should have accompanying headers, regardless of the file hierarchy.

All constants and [non-inline] documentation should be done in header files.

+ This includes when the values will be used to index arrays. In this case, values should be assigned in the typedef.
+ Configuration variables are still constants.

---

Header files should be broken up into "major sections", in the following order:

1. CONFIGURATION VARIABLES
2. PIN DEFINITIONS
3. ENUMERATIONS
4. [other misc sections, as needed]
5. AVAILABLE FUNCTIONS
6. INTERNAL FUNCTIONS

Each of these major sections should be followed by two empty lines, and should be labeled like so:
```C++
/////////////////////////
// MAJOR SECTION TITLE
/////////////////////////
```


#Naming Schemes

+ Variables (including constants), arguments, and typedefs should use underscores to separate words.
+ Functions should include no underscores.

---

+ Function names should use camel case.
+ Function arguments should be in all lowercase.
+ Constants should be in all caps.
+ Typedefs should be in all lowercase.
+ All other variables should capitalize the first letter of every word.

---

+ Functions that do actions should be in the form verb - noun.
+ Functions that return a boolean should in the form noun - adjective.
	+ This is of lower priority than the form "verb - noun". A function returning a boolean should only be named in this fashion if it passively acquires this value.

---

Global variables and constants should be named such that their originating module is obvious.

---

In general, "Begin" should be used for actions, and "Start" for locations and times.

+ "End" is the preferred complement to "Begin".
+ "Stop" is the preferred complement to "Start".
+ These terms may be changed if context makes them ambiguous, implies another use, or is contrary to external documentation.
	+ For example, `sendEndCommand()` should be renamed if the command being sent is referred to as "STOP" in the datasheet.


#Abbreviations

Easily-understood abbreviations should be used when practical. Some examples are:

+ "Str" for "String"
+ "Curr" for "Current"
+ "Prev" for "Previous"
+ "Init" for "Initialize"
+ "Config" for "Configuration" or "Configure"
+ "Cal" for "Calibration" or "Calibrate"

Uncommon or arbitrary abbreviations should only be used if unambiguous in their context. Some examples:

+ "Int" for "Interrupt", in the case of "CHIP_INT_PIN"
+ "Dir" for "Direction", in the case of "Motor_Dir"
+ "SS" for "Slave Select", in the case of "SPI_SS_PIN"

---

Function names should remain unabbreviated if doing so reduces readability. For example:

+ "getRecordingDuration()" should not be abbreviated to "getRecDur()".
+ "calibrateAudioInput()" should not be abbreviated to "calAudioIn()".

---

The abbreviations "In" and "Out" should only be used at the end of names.

For example, "In_Data" is ambiguous without determining the data type, whereas "Data_In" clearly represents input data.


#Data Types and Usage

These data types should not be used:

+ `boolean` (`bool` should be used instead)
+ `short` and `unsigned short`
+ `word`
+ `unsigned char` (unless potentially storing extended ASCII)

---

+ Variables storing exclusively ASCII characters should always be type `char`.
+ Unsigned variables that are highly unlikely to approach 255 should be of the type `byte`.
	+ This type may be used to implicitly limit values, so long as the maximum value is not critical.
+ Similarly, variables that are highly unlikely to approach maximum values should be of type `int` or `unsigned int`.
+ Values that may potentially become larger than `int` would allow should use `long` or `unsigned long`.
	+ The `unsigned long` type should be used for all variables dealing with timing, with the exception of short delay constants.

---

+ Unsigned types should be used if the variable cannot logically take a negative value.
+ Explicit data types (such as `uint8_t`) should be used if the bit width is critical.
	+ If the value in question would logically be written on paper in binary or hex, these types should be used.
		+ An exception can be made if the value is used to mask a value of a non-explicit type.
	+ Absolute pointers should always use unsigned explicit data types.

---

Variables should always specify a type upon declaration.

+ Variables should be initialized on declaration, assuming a default value is logical and practical.
+ Global variables should be initialized before `loop()` executes.
+ The `typedef` keyword should be used when defining enumerations.

---

Constant values should not use `#define` statements. A `const` or enumeration should be used instead.

+ An exception may be made when the value must be used by the preprocessor. In this case, the defined macro should be used minimally, additionally assigning a `const` for all non-preprocessor usage.
	+ The macro should be prepended with "MACRO_".

---

Enumerations should be used whenever they make sense.

+ If only two states exist, an enumeration should still be used unless the value is a boolean.
	+ For example, "Motor_Direction" should be an enumeration, and "Motor_Enabled" a boolean. In this example, the use of an enumeration could be avoided by renaming "Motor_Direction" to "Motor_Forward". However, this should not be done as the name "Motor_Forward" implies the motor is also enabled.


#Documentation and Comments

Header files should describe their functionality at the top of the file, like so:
```C++
/* So-and-so Module
 *
 * Used to do task A, B, and C
 *
 * [Optional] further description of these tasks go here.
 *
 * [Optional] additional sections describing technical details and usage go here.
 *
 * Written by Author <email@example.com>,
 *            Author <email@example.com>
 */
```

+ Note that not all sections use sentence structure.
+ If the file is a main library file, the word "Module" should be replaced with "Library".
+ If the file is the main project file, the word "Module" should be omitted entirely.

---

Function documentation (located in header files) should be organized like so:
```C++
type functionName(arg_type argument, arg_type argument);
/*
 * Summary of function
 * [Optional] information on when to be used
 *
 * [Optional] additional information goes here. There may be multiple of these sections.
 *
 * [The following lines are to be used as needed.]
 * Affects Global_Variable_A and Global_Variable_B
 * INPUT:  Argument 1 description (X-indexed)
 *         Argument 2 description
 * OUTPUT: Return description
 */
```

+ Note that not all sections use sentence structure.
+ Arrays should be labeled as either 0 or 1 indexed as needed.

---

Inline comments should be in the form:
```C++
// Example comment
```

+ End-of-line comments should be aligned two spaces beyond the longest line of text in the block.

---

"To do" comments should be written as:
```C++
// TODO
// [Optional] additional comment(s) explaining what to do
```

+ The same format should be kept for "to do" comments within block comments.

---
