# ARC Game Assets Packing Tool (v1.0)
ARC (Archive File) is a lightweight, flat-file binary container format designed to bundle game assets such as maps and textures into a single distribution file. Its design is heavily inspired by the classic WAD (Where's All the Data?) format used in the Doom engine.  
## File Structure Overview  
The format follows a "Header-Data-Directory" architecture. The metadata (Lumps Table) is stored at the end of the file to allow efficient streaming of the header and single-pass data writing during packing.  

| Section     | Description                                                                     |
|-------------|---------------------------------------------------------------------------------|
| Header      | Contains the magic signature, file count, and pointer to the directory.         |
| Data Lumps  | Raw binary data of assets (image bytes, map arrays, etc.) stored contiguously.  |
| Lumps Table | A directory (index) containing offsets, sizes, and names for every asset.       |
