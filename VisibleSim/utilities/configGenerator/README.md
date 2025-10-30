# VisibleSimConfigGenerator

Generate connected configurations for [VisibleSim simulator](http://projects.femto-st.fr/projet-visiblesim/).

### Compilation

To compile VisibleSimConfigGenerator, go into the `build/` folder and enter `make`.

```
$ cd build/
$ make
```

### Usage

VisibleSimConfigGenerator works as following:

```
Usage: VisibleSimConfigGenerator -r <target robot system> -s <lattice size (2D or 3D)> -o <ouput xml file> -t <topology to generate> <topology parameter> [options]
Supported target robot systems(<target robot system>): 
  bb: blinkyblocks
  sb: smartblocks
  c2: catoms2d
  c3: catoms3d
Lattice size format:
  "x,y" for 2D configurations
  "x,y,z" for 3D configurations
Supported topologies (<topology to generate>): 
  r: random (parameter: size)
  b: networkball (parameter: radius)
  l: line (parameter: length)
Options:
-c: enable color
-h: print this usage and exit
```

### Example

Below is a full example of how to compile and use VisibleSimConfigGenerator. In this example, a random and connected configuration composed of 30 modules arranged in a 10x10x10 lattice is generated for the Blinky Blocks modular robot system in the file `build/example.xml`.

```
$ cd build/
$ make
$ ./visibleSimConfigGenerator -r blinkyblocks -s 10,10,10 -o example.xml -t 
random 30
Target robot: blinkyblocks
Topology: random, parameter: 30
Lattice size: 10,10,10
Output: example.xml
Color: false
Generating random configuration of size 30...
Configuration with 30 modules generated!
exporting to VisibleSim...
Configuration exported in example.xml
```

Below is a screenshot of VisibleSim loaded using the generated configuration file `build/example.xml`:

![](screenshots/visiblesim-example.png?raw=true)

*Screenshot of VisibleSim when loaded with the configuration file `build/example.xml`.*