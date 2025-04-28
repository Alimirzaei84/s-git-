# s-git

**Ali Mirzaei Feizabiadi** 

---

## Overview

s-git is a lightweight, educational implementation of a subset of Git’s functionality, written in C. It was developed as part of the Fundamentals of Programming (FOP) course in two phases:

- **Phase 1 (v1.0.0):** Core version-control commands: `init`, `add`, `reset`, `status`, `commit`, `log`, `branch`, and `checkout` citeturn1file0turn6view0.
- **Phase 2 (v2.0.0):** Advanced features: `revert`, `tag`, `tree`, `stash`, `pre-commit`, `grep`, `diff`, and `merge` citeturn1file1turn6view0.

This tool offers a hands-on way to explore how version control systems manage snapshots, branches, and history under the hood.

## Features

### Phase 1 (v1.0.0)
- **Initialization**: Create a new repository with `sgit init`.
- **Staging**: Add files or directories to the staging area (`sgit add <path>`).
- **Unstaging**: Remove from staging (`sgit reset <path>`).
- **Status**: View workspace and index status (`sgit status`).
- **Committing**: Record staged changes (`sgit commit -m "message"`).
- **Log**: Browse commit history (`sgit log`).
- **Branches**: Create (`sgit branch <name>`) or switch (`sgit checkout <branch>`) branches.

### Phase 2 (v2.0.0)
- **Revert**: Undo a commit by creating a new inverse commit (`sgit revert <commit-id>`).
- **Tags**: Annotated (`sgit tag -a <name> -m "msg"`) or list tags (`sgit tag`).
- **Commit Graph**: Display commit tree (`sgit tree`).
- **Stash**: Temporarily shelve changes (`sgit stash push` / `pop` / `list`).
- **Pre-commit Hooks**: Define and run checks before committing (`sgit pre-commit add <hook>` / `sgit pre-commit`).
- **Search**: Grep through commits or files (`sgit grep -p <word> [-c <commit>]`).
- **Diff**: Diff between files or commits (`sgit diff` commands).
- **Merge**: Merge branches with conflict markers (`sgit merge -b <base> <target>`).

## Installation

1. **Prerequisites**: GCC (or compatible C compiler) and a Unix-like shell.
2. **Clone repository**:
   ```bash
   git clone https://github.com/Alimirzaei84/s-git-.git
   cd s-git-
   ```
3. **Compile**:
   ```bash
   gcc -o sgit sgit.c
   ```
4. **Make executable available globally** (optional):
   ```bash
   mv sgit /usr/local/bin/
   ```

## Usage Examples

```bash
# Initialize a new s-git repository
sgit init

# Stage all C files
sgit add *.c

# View status
sgit status

# Commit changes
sgit commit -m "Initial implementation"

# Browse history
sgit log

# Create and switch branch
sgit branch feature-x
sgit checkout feature-x

# Revert a bad commit
sgit revert a68b494

# List and view tags
sgit tag
sgit tag show v1.0.0

# Display commit graph
sgit tree

# Stash local changes
sgit stash push -m "WIP"
sgit stash list
sgit stash pop

# Run pre-commit checks
sgit pre-commit

# Search for a term in history
sgit grep -p TODO

# Show diff between files
sgit diff -f old.c new.c --line1 1-50 --line2 1-50

# Merge branches
sgit merge -b main feature-x
```

## Release History

- **v2.0.0** – *Feb 5, 2024* – Added Phase 2 commands: advanced operations, hooks, and merge support citeturn6view0.
- **v1.0.0** – *Feb 5, 2024* – Initial Phase 1 implementation of core version-control commands citeturn6view0.

## Contributing

Contributions are welcome! Feel free to fork the repository, implement improvements or new commands, and submit a pull request.

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

