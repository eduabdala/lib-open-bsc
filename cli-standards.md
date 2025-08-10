# Command Line Interface Standards

## 1. Utility Argument Syntax

This section describes the syntax conventions used for defining command-line interfaces (CLIs) following the GNU style.

The general syntax of a utility invocation is:

```text
utility_name [-a] [-b] [-c option_argument] [-d|-e] [-f[option_argument]] [operand...]
```

or, using long options:

```text
utility_name [--alpha] [--beta] [--charlie option_argument] [--delta|--echo] [--foxtrot[=option_argument]] [operand...]
```

### Do **not** use:

```text
utility_name [-a] [-b] [-c=option_argument] ...
utility_name [--charlie=option_argument] ...
```

These forms are discouraged unless the implementation explicitly supports optional arguments using = syntax.

---

## 2. Syntax Elements

- `utility_name`: The name of the executable or command-line tool.
- `-a`, `-b`: Short options (single-character switches).
- `--alpha`, `--beta`: Long options.
- `option_argument`: An argument required or optionally provided to an option.
- `operand`: Non-option arguments, typically input files, values, or commands.

---

## 3. Guidelines

### 3.1 Utility Naming
- Utilities should have names between **2 and 9 characters**.
- Names should use **lowercase letters and digits only**.
  
Example:
```bash
pmdtr --pid 0x00A1 --command V -rd
```

### 3.2 Options
- **Short options**:
  - Begin with a single hyphen (`-`), followed by a single alphanumeric character.
  - Grouped short options are allowed: `-abc`
  - If an option requires an argument, it must be provided as a **separate argument**, unless explicitly optional.
  
- **Long options**:
  - Begin with two hyphens (`--`) and a readable name.
  - Option-arguments may be passed as `--option value` or, when optional, `--option=value`.

### 3.3 Option-Arguments
- **Mandatory option-arguments** must appear as the **next argument**:
  ```bash
    utility_name -c arg      # ✔ Correct
    utility_name -c=arg      # ❌ Not recommended
  ```
- **Optional option-arguments**, if allowed, must be adjacent (no space):
  ```bash
  utility_name -fvalue     # ✔ Allowed if -f takes optional argument
  utility_name -f value    # ❌ Ambiguous
  ```

### 3.4 Operands
- Operands follow all options and are used as inputs, targets, or other values required by the utility.
- Use `'--'` to signal the end of options:

  ```bash
  utility_name -- -filename
  ```

### 3.5 Repetition and Exclusivity
- Options may be **mutually exclusive**. If so, they must be documented.
  ```bash
  utility_name [-d | -e]   # -d and -e cannot be used together
  ```
- Multiple occurrences of an option (like `-f arg -f arg2`) are allowed only if the utility explicitly supports it.

---

## 4. Example SYNOPSIS

```
pmdtr [-a] [-v level] [-c command] [-r|-d] [--pid PID] [--vid VID] [file...]
```

Where:
- `-a`: enable alpha mode
- `-v level`: set verbosity level
- `-c command`: execute command
- `-r|-d`: mutually exclusive reset/debug modes
- `--pid`, `--vid`: device identification
- `file...`: one or more files as operands

---

## 5. Option Grouping Rules

- Accept grouped short options:
  ```bash
  pmdtr -rv
  ```
- If one option in a group requires an argument, the group must stop before it:
  ```bash
  pmdtr -r -c CMD     # ✔ OK
  pmdtr -rcCMD        # ✔ if -c takes optional argument
  pmdtr -rc CMD       # ❌ ambiguous
  ```

---

## 6. Summary of Guidelines

| # | Guideline |
|---|-----------|
| 1 | Utility names: 2–9 lowercase alphanumeric characters |
| 2 | Use single-character options, prefixed with `-` |
| 3 | Long options prefixed with `--`, must be unique |
| 4 | Use `--` to end option parsing |
| 5 | Short options may be grouped |
| 6 | Each option-argument must be a separate argument (if mandatory) |
| 7 | Optional option-arguments must be adjacent (no space) |
| 8 | All options precede operands |
| 9 | Use `-` as operand to mean stdin/stdout when applicable |
| 10 | Document any mutually exclusive options clearly |

---

## 7. Recommended Practice

Use consistent GNU-style notation when documenting command-line usage:

```
utility_name [-a] [-b] [-c option_argument] [--long-option] [operand...]
```

Avoid embedding arguments using = unless the option explicitly allows optional arguments in GNU style.
