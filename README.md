# Libcj

Libcj is a simple header only json stringify and parsing library written in c, released under the [AGPL-3.0-only](https://spdx.org/licenses/AGPL-3.0-only.html).

[cj.h](./cj.h)

## Usage

Important notice

The general usage is to include the implementation in you "main.c" file
```c
#define IMPL_CJ
#include "cj.h"
```
and include only the header in other files like
```
#include "cj.h"
```

Examles are provided in the [examples directory](./examples).

* [Parsing json into a struct](./examples/parse_object.c)
* [Decoding JSON](./examples/decode.c)
* [Encoding JSON](./examples/encoder.c)

For more inspiration checkout the tests in [tests](./tests).

## License

```
Copyright (C) 2024 Mario Aichinger <aichingm@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

```

