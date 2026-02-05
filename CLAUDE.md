# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

pgAdmin3 LTS is a maintenance fork of pgAdmin3, a GUI administration and development platform for PostgreSQL. This codebase is written in C++ using wxWidgets for the UI framework. While pgAdmin3 has been superseded by pgAdmin4, this fork exists to preserve the codebase and provide compatibility with modern PostgreSQL versions (currently supports up to PostgreSQL 18).

**Important**: This is a maintenance-only project with no active leadership or commitments to maintain future PostgreSQL compatibility. No binaries are provided; users must build from source.

## Build System

### Prerequisites
- **Linux/Unix**: wxGTK 2.8.x or 3.0.x, libxml2 2.6.18+, libxslt 1.1.x+, libssh2 1.10.0+, PostgreSQL 10.x+, GNU automake 1.9+, GNU autoconf 2.59+, GNU gcc 3.4+
- **Windows**: Microsoft Visual C++ 2019, Windows 10 Platform SDK, WiX toolset v3, wxMSW 3.0.5+, libssh2 1.10.0+, PostgreSQL 10.19+
- **Mac OS X**: XCode 2.0+, wxMac 2.8.x, libxml2 2.6.18+, libxslt 1.1.x+, libssh2 1.10.0+, PostgreSQL 10.x+

### Building on Linux/Unix

```bash
# Install dependencies (Debian/Ubuntu/Mint)
apt-get install libwxgtk3.0-dev wx3.0-headers wxgtk3.0 wx3.0
apt-get install libssh2-1 libssh2-1-dev libgcrypt20 libgcrypt20-dev libjson-perl libpq-dev

# Bootstrap (required when building from Git checkout)
bash bootstrap

# Configure
./configure --prefix=/opt/pgadmin3bigsql --with-libgcrypt --with-wx-version=3.0 CFLAGS=-fPIC CXXFLAGS=-fPIC

# Build
make -j8

# Install
sudo make install
```

**CentOS/RedHat variant**:
```bash
yum install wxGTK3 wxGTK3-devel
yum install libssh2 libssh2-devel libxml2 libxml2-devel libxslt libxslt-devel openssl-devel

bash bootstrap
./configure --prefix=/opt/pgadmin3bigsql --with-wx-version=3.0 CFLAGS=-fPIC CXXFLAGS=-fPIC --with-pgsql=/usr/pgsql-18 --without-sphinx-build
make -j8
sudo make install
```

### Building on Windows

- Set environment variables: `%WXWIN%` (wxMSW location), `%PGDIR%` (PostgreSQL location), `%LIBSSH2DIR%` (libssh2 location)
- Open `pgAdmin3.sln` in Visual Studio and build the required targets
- Copy prerequisite DLLs to the executable directory

### Building on Mac OS X

```bash
bash bootstrap
./configure --enable-appbundle
make all
make install
```

Use `pkg/mac/debug-bundle.sh` to create a development appbundle (`pgAdmin3-debug.app`) that doesn't need rebuilding on each compilation.

### Additional Build Targets

```bash
make doc      # Build HTML documentation (requires Sphinx)
make xrc      # Embed XRC resources
make style    # Apply astyle formatting to enforce coding style
```

## Code Architecture

### Factory Pattern

The codebase uses a factory-based architecture for database objects and UI components:

- **Object Hierarchy**: Database objects (tables, views, functions, etc.) inherit from `pgObject` base class
- **Factories**: Each object type has an associated factory (e.g., `pgTableFactory`) derived from `pgaFactory`
- **Collections**: Object collections (e.g., `pgTableCollection`) group related objects
- **Collection Factories**: Create collections (e.g., `pgTableCollectionFactory` derived from `pgaCollectionFactory`)

**Naming Conventions**:
- `foo` = object of type Foo
- `fooFactory` = creates foos
- `fooCollection` = collection of foos
- `fooCollectionFactory` = creates a fooCollection
- `fooObject` = object residing under/in foo
- `fooObjCollection` = collection of fooObjects

### Directory Structure

```
pgadmin/
├── agent/          # pgAgent job scheduler support
├── ctl/            # Custom wxWidgets controls
├── db/             # Database connection layer (pgConn, pgSet)
├── dd/             # Database Designer
├── debugger/       # PL/pgSQL debugger
├── dlg/            # Dialog classes (properties, wizards)
├── frm/            # Main frames and windows (frmMain, frmQuery)
├── gqb/            # Graphical Query Builder
├── hotdraw/        # Drawing framework for Database Designer
├── include/        # Header files (organized by component)
├── ogl/            # Object Graphics Library
├── pgscript/       # pgScript scripting language
├── schema/         # Database object classes (pgTable, pgView, etc.)
├── slony/          # Slony-I replication support
├── ui/             # XRC UI resource files
└── utils/          # Utility classes
```

### Database Connection Layer

- **pgConn**: Main PostgreSQL connection class (wraps libpq)
- **pgSet**: Result set class for queries
- **pgQueryThread**: Background query execution
- Connection establishment handles SSL modes, SSH tunneling (if compiled with OpenSSL/libgcrypt)

### Schema Objects

All PostgreSQL database objects are represented by classes in `pgadmin/schema/`:
- Server objects: `pgServer`, `pgDatabase`, `pgTablespace`, `pgRole`
- Schema objects: `pgSchema`, `pgTable`, `pgView`, `pgFunction`, `pgSequence`, etc.
- Constraints: `pgCheck`, `pgForeignKey`, `pgIndexConstraint`
- Each object class has a corresponding factory registered in `frmMain::CreateMenus()`

### UI Framework

- **frmMain**: Main application window with object browser tree
- **frmQuery**: SQL query editor
- **frmEditGrid**: Edit table data in grid
- **frmConfig**: Configuration file editor (postgresql.conf, pg_hba.conf, pg_ident.conf)
- Dialog classes (`dlg*`) provide property editors for database objects
- Custom controls (`ctl*`) extend wxWidgets with specialized widgets

## Adding New Features

### Adding Actions to Main Window

1. Create the `frmXXX` class for the new function
2. Create an `actionFactory` (or `contextActionFactory` for context menus) derived factory
3. Register it in `frmMain::CreateMenus()` - never modify `menu.h` directly
4. Implement `CheckEnable()` to check if action applies to current object
5. Implement `StartDialog()` to bring up the dialog

See `frmGrantWizard` or `frmEditGrid` as simple examples.

### Adding Objects to Object Tree

1. Implement the class with proper hierarchy (see `pgCast` and `dlgCast` as examples)
2. Instantiate a collection in the parent `pgXXX` object where it should appear
3. Register the factory in the appropriate location

## PostgreSQL Version Compatibility

Recent changes adapted the code for PostgreSQL internal changes through version 18:

- **No more `relhasoids`** in `pg_class`
- **Sequence changes** (PG 11+): No more `cache_value`, `is_cycled`, `is_called` in sequence objects
- **No more `adsrc`** in `pg_attrdef`: Use `pg_catalog.pg_get_expr(adbin, adrelid)` instead
- **Declarative Table Partitioning**: Full DDL support
- **No more `datlastsysoid`** in `pg_database`

When adding support for new PostgreSQL versions, check `pgadmin/utils/pgfeatures.h` for version detection and feature flags.

## Coding Standards

- Use `./enforce-style.sh` or `make style` to apply astyle formatting
- Follow the existing naming conventions and patterns
- All new code should maintain consistency with the established architecture
- Database operations should use the `pgConn` and `pgSet` classes
- UI code should follow the factory pattern for object creation and management

## Development Workflow

1. Make changes to source files
2. Run `make style` to enforce coding style
3. Build with `make -j8`
4. Test changes
5. The build system uses automake with modular makefiles (`module.mk` in each subdirectory)

## Visual Studio Build

The Windows build uses MSBuild with:
- Solution file: `pgAdmin3.sln`
- Project properties: `pgAdmin3.props`
- Build targets: `pgAdmin3.targets`
- Resource file: `pgAdmin3.rc`

Dependencies are managed through environment variables (`%WXWIN%`, `%PGDIR%`, `%LIBSSH2DIR%`).

## Image Resources

PNG images in `pgadmin/include/images/` are converted to embeddable C format using the `xtra/png2c/png2c` tool during build. The Makefile automatically generates `.pngc` files from `.png` sources.

## Internationalization

- Translation files in `i18n/` directory (47 language directories)
- POT files: `pgadmin3.pot`, `pgadmin3-release.pot`
- Language files: `languages.po`, `languages.hwx`
- String extraction: Use `./stringextract` script
- String merging: Use `./stringmerge` script
