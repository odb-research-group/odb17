Building with the documentation
===============================

```
cd <project_root>
virtualenv env
. env/bin/activate
pip install -r docs/requirements.txt
mkdir -p build
cd build
cmake ..
cmake --build .
```

Building **WITHOUT** the documentation
======================================

```
cd <project_root>
mkdir -p build
cd build
cmake .. -DBUILD_DOCS=Off
cmake --build .
```

------------------- OR (e.g.)

mkdir -p build/CodeBlocks
cd build/CodeBlocks
cmake ../../ -G "CodeBlocks - Unix Makefiles"
codeblocks odb.cbp

------------------- OR

use a different CMAKE Generator suitable to you environment
