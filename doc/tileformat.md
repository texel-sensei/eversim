# Tile format

A tile descriptor file is formatted in JSON. Each type of tile has a unique name, that is referenced by the level-id-table.
The name of a tile is not explicitly saved in the JSON file itself, but is instead the (relative) path to the file,
starting at the base directory.

> E.g. base directory is `tiles`, then the tile `env/dirt` should be in file `base/env/dirt.json`.

## Json attributes

The format does not have any mandatory attributes. Possible attributes are the following:

name|type|explanation|default
----|---------------|-----------|-------
texture | texture name | The texture identifyer of the tile | no texture
has_collision | boolean | True if bodies should be able to collide with this tile | false
parent_tile | tile name | See **Inheritance** | no parent

Unknown attribute names may trigger a warning, but are otherwise ignored.

## Inheritance

Any tile may declare any other tile as a parent. Each tile may have only a single parent or none.
If a tile has a parent, it automatically has all the attributes of its parent tile.
Any attibutes the child tile declares overwrite the corresponding attributes of the parent.
Cyclic inheritance is an error and causes the tile to fail loading.