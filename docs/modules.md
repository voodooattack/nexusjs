# Modules

## Syntax

Nexus.js supports the ES6 module syntax. It does not come with built-in support for other module packaging schemes (CommonJS, AMD, etc)

Example:
```js
import * as MyPackage from 'my-package'; // import a package 
import { MyClass } from './myclass'; // import using a relative path
export * from '/full/path/to/file'; // export using an absolute path
export const myInt = 42; // export a constant
```
## import.meta

You can use `import.meta` to obtain metadata about the current file.

```js
console.log(import.meta.filename); // full path to the current file.
console.log(import.meta.dirname); // full path to the directory containing the current file.
console.log(import.meta.url); // same as `import.meta.filename`, provided for compatibility.
```

## URLs

You can import URLs directly in your code, like so:

```js
import { h } from 'https://unpkg.com/preact/dist/preact.esm.js';
```

## NPM  **(Planned, WIP)**

Or from NPM:

```js
import _ from 'npm://lodash@latest'; 
```

## Dynamic Import

Nexus.js also supports dynamic `import` statements, which return a `Promise`:

```js
  import('my-package').then(...);
```

You can also use `await` in an async function:

```js
async function f() {
  const myPackage = await import ('my-package');
}
```

## Package Installation **(Planned, WIP)**

Nexus.js will install the required dependencies at run-time. There is no need for an explicit installation step.

Packages are installed at `~/.nexus/cache/${source}/${package}@${version}`.

## package.json **(Planned, WIP)**

If a version is not specified explicitly in the `import` directive, Nexus.js will look for the nearest `package.json` file as a secondary source of truth regarding the requested package versions.

This can be used to override the package versions in a specific subdirectory of your application, if so desired.

 

