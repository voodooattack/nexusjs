# Nexus.FileSystem

A namespace holding file-system related functions. The API is very basic right now, but more will be added soon.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `separator` | `string` | The operating system's preferred path separator. |
| `FileType` | `Object` | `Directory`, `Block`, `FIFO`, `NotFound`, `Regular`, `Reparse`, `Socket`, `Symlink`, `Unknown` |
| `Permissions` | `Object` |  `AllAll`, `GroupAll`, `GroupExec`, `GroupRead`, `GroupWrite`, `OwnerAll`, `OwnerExec`, `OwnerRead`, `OwnerWrite`, `OthersAll`, `OthersExec`, `OthersRead`, `OthersWrite`, `SetGID`, `SetUID`, `StickyBit` |

## Methods
| Signature | Description |
|----------| ----------- |
| `stat(path: string): Promise<StatObject>` | Returns the status of a file or directory. |  
| `join(...pathParts: string[]): string` | Join one or more path segments into a single path string using the preferred system delimiter. |
| `absolute(...pathParts: string[]): string` | Join one or more path segments into a single path string using the preferred system delimiter, returning an absolute path. |

# StatObject

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `type` | `Nexus.FileSystem.FileType` | A bit-mask of `Nexus.FileSystem.FileType` values.
| `permissions` | `Nexus.FileSystem.Permissions` | A bit-mask of `Nexus.FileSystem.Permissions` values.
| `lastMod` | `Date` | Last modified date. |