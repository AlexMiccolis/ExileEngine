# Exi::Runtime::Filesystem
1. [Path](#Path)
2. [Filesystem](#Filesystem)
3. [File Control](#File-Control)
4. [File Handle](#File-Handle)

## Path

The `Runtime::Path` class is an alternative to the `std::filesystem::path` template.
At it's core, the Path class is a `std::string` along with some contextual information
about the contained path. Upon construction from a string, the path is normalized.
During normalization, extra directory separators are condensed and forward slashes
are converted to backslashes. Forward slashes are *__forbidden__* from being used
in *filenames* and may only be used as directory separators.

## Filesystem

The `Runtime::Filesystem` class creates a k-ary tree of virtual mount points
which represents a virtual filesystem. Virtual paths are translated into physical 
(disk) paths by walking this tree. Translations are cached in an LRU cache to avoid 
having to re-translate recently used virtual paths. Modifying the VFS tree will cause 
the translation cache to be cleared completely.

## File Control

`Runtime::FileControl` represents an open file stream or a memory-mapped file that can
be referenced by multiple file handles.

## File Handle

`Runtime::FileHandle` is a class that represents a handle to an open file. Instances are
created by the `Filesystem::Open` method, and each one is tied to a `FileControl`
instance via a shared pointer. The `FileHandle` class exposes a multitude of 
reading/writing functions and is the main interface for file I/O.

File handles themselves may not be shared across threads. Each thread must open its own
handle to a file. File operations through handles are thread-safe, and multiple threads 
may read and write to the same file through separate handles.

