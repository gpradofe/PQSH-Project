# Project 01: Process Queue Shell

This is [Project 01] of [CSE.30341.FA23].

## Students

- Gustavo Aniceto (gpradofe@nd.edu)

## Errata for Application Code

This document details the known issues and areas of improvement for our application.

### Table of Contents

- [Error Handling](#error-handling)
- [Concurrency Issues](#concurrency-issues)
- [Process Handling](#process-handling)
- [Memory Management](#memory-management)
- [Undefined Behavior](#undefined-behavior)
- [User Interface & Input](#user-interface--input)
- [Code Maintenance](#code-maintenance)
- [Quirks](#quirks)

### Error Handling

- **parse_command_line_options function**: Assumes every option is followed by an argument, leading to potential out-of-bounds access for options `-n`, `-p`, and `-t`.

### Concurrency Issues

- **Queue Operations**: Absence of mutex or locks can lead to race conditions if functions are called concurrently.

### Process Handling

- **process_start function**: If `fork()` succeeds but `execvp` fails, the child process doesn't exit cleanly.
- **Waiting Queue**: Processes with a PID of 0 are assumed to be unstarted. Misplacement can lead to wrong interpretation.

### User Interface & Input

- **Command Parsing**: Commands with multiple spaces or tabs can be parsed incorrectly.
- **Argument Limit**: Commands with arguments beyond `MAX_ARGUMENTS` drop excess arguments without user notification.

### Code Maintenance

- **Magic Numbers**: Numbers like `250000` are used without context or named constants.
- **Global Variables**: Some functions implicitly depend on them, making code harder to test and refactor.

### Quirks

- **rdrn Behavior**: The `rdrn_scheduler` function sometimes executes faster than expected, and the reason remains unidentified.

# Project Presentation Recording:

[Project Recording]: https://drive.google.com/file/d/1PLCqmwKpcnkO-rpgdfyet0g4QhmOCNLI/view?usp=sharing
