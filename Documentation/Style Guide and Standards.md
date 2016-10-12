#About This Guide

This guide is an attempt to have a single answer as to how this project should be formatted.
It covers a large range of stylistic decisions, ranging from general file layout to variable naming.
Both code and documentation of all forms are covered.

Since its inception, this guide has also been expanded such that it covers coding standards.

Note that this guide is not intended to be the answer to everything in life;
it is purely to keep these files consistent.
If you disagree with these points, feel free to ignore them for your own projects.


#General Notes

+ This style guide should define points as "should be" or similar.
  Absolutes shouldn't be in here because I'll probably make mistakes.

+ An effort should be made to keep lines under about 100 characters long.
	+ This assumes a tab width of 2.
	+ Lines should be broken on punctuation, if possible.
	+ If the intuitive understanding of the line is inhibited by this, it should not be split.
	+ Lines within deeply nested structures should not be split,
	  though such structures should be refactored.


#General Formatting

+ All major sections*  should be followed by two empty lines.
	+ *Loosely defined as a top-level section that has a really sweet header.
	+ This should not be followed at the end of the file.
+ All sections (regardless of hierarchy) should be preceded by one empty line.
	+ This is, of course, overridden for the section beneath a major section.
	+ This is also not the case for the first lines of a file.

+ Tabs are to be used for indentation and spaces for alignment. There are no excuses.
	+ Changing the tab width should not break anything under any circumstances.
+ Tabs and spaces are to be mixed if called for in the situation.

+ Constants should not use ```#define``` statements. ```enum``` and ```const``` should be used instead.

+ Enumerations should always be defined using the ```typedef``` keyword.


#Header Files

+ All source files should have accompanying headers, regardless of the file hierarchy.
+ All constants and [non-inline] documentation should be done in header files.
	+ This includes argument type specification.
	+ This includes when the values will be used to index arrays.
	  In this case, values should be assigned in the typedef.
+ Configuration variables are still constants. They should be placed at the top of header files.

+ Sections should be in the following order:
	1. CONFIGURATION VARIABLES
	2. PIN DEFINITIONS
	3. ENUMERATIONS
	4. [other misc sections, as needed]
	5. AVAILABLE FUNCTIONS
	6. INTERNAL FUNCTIONS


#Naming Schemes

+ Variables (including constants), arguments, and typedefs should use underscores to separate words.
+ Functions should include no underscores.

+ Function names should use camel case.
+ Function arguments should be in all lowercase.
+ Constants should be in all caps.
+ Typedefs should be in all lowercase.
+ All other variables should capitalize the first letter of every word.

+ Global variables and constants should be named such that their originating module is obvious.

+ Functions that do actions should be in the form verb - noun.
+ Functions that return a boolean should in the form noun - adjective.


#Data Types and Usage

+ These data types should not be used:
	+ ```boolean``` (```bool``` should be used instead)
	+ ```short``` and ```unsigned short```
	+ ```word```
	+ ```unsigned char``` (unless potentially storing extended ASCII)

+ Variables storing exclusively ASCII characters should always be type ```char```.
+ Unsigned variables that are highly unlikely to approach 255 should be of the type ```byte```.
	+ This type may be used to implicitly limit values, so long as the maximum value is not critical.
+ Similarly, variables that are highly unlikely to approach maximum values should be of type
  ```int``` or ```unsigned int```.
+ Values that may potentially become larger than ```int``` would allow should use ```long```
  or ```unsigned long```.
	+ The ```unsigned long``` type should be used for all variables dealing with timing,
	  with the exception of 

+ Unsigned types should be used if the variable cannot logically take a negative value.
+ Explicit data types (such as ```uint8_t```) should be used if the bit width is critical.
	+ If the value in question would be written on paper in base 2 or 16, these types should be used.
	+ Absolute pointers should always use unsigned explicit data types.
+ Enumerations should be used whenever they make sense. This includes where there are only two states.

#Documentation and Comments

+ Header files should describe their functionality at the top of the file, like so:
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

+ Function documentation (located in header files) should be organized like so:
	```C++
	type functionName(arg_type argument, arg_type argument);
	/*
	 * Summary of function
	 * [Optional] information on when to be used
	 *
	 * [Optional] additional information goes here. There may be multiple of these sections.
	 *
	 * [The following lines are to be used as needed.]
	 * Affects Variable_A and Variable_B
	 * INPUT:  Argument 1 description (X-indexed)
	 *         Argument 2 description
	 * OUTPUT: Return description
	 */
	```
	+ Note that not all sections use sentence structure.
	+ Arrays should be labelled as either 0 or 1 indexed as needed.

+ Inline comments should be in the form:
	```C++
	// Example comment
	```
	+ End-of-line comments should be aligned two spaces beyond the longest line of text in the block.

+ "To do" comments should be written as:
	```C++
	// TODO
	// [Optional] additional comment(s) explaining what to do
	```
	+ The same format should be kept for "to do" comments within block comments.

+ Major sections of code should be labeled like so:
	```C++
	/////////////////////////
	// MAJOR SECTION TITLE
	/////////////////////////
	```

+ Minor sections of code should be labeled using standard inline comment form, if needed.