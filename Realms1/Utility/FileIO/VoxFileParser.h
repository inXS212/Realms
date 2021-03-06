#pragma once
#include "Voxelite.h"
#include <string>
#include "../../Base/RlmsException.h"

namespace rlms {
	struct InvalidVox : public RlmsException {
		InvalidVox (std::string t_type, std::string t_description = "") noexcept : RlmsException (t_type, t_description) {};
	};

//the file has not been found or is not name the right way
	struct InvalidVoxFile : public InvalidVox {
		InvalidVoxFile () noexcept : InvalidVox ("Invalid vox file.", "Make sure your file exists and have a valid format. (Vox files) ") {};
	};

//the file structure is invalid
	struct InvalidVoxFileFormat : public InvalidVox {
		InvalidVoxFileFormat () noexcept : InvalidVox ("Invalid vox file format.", "Make sure your file have a valid format. (Vox file format) ") {};
	};

//the IVoxel format of this file is not yet supported, make sure to export the voxel based mesh, and not just save it
	struct UnsupportedVoxFile : public InvalidVox {
		UnsupportedVoxFile () noexcept : InvalidVox ("Unsupported vox file.", "Make sure to export your file (export to vox file in magicaVox, ). This version of vox format is not supported yet.") {};
	};

	class VoxFileParser {
	public:
		static void Load (Voxelite& data, std::string const& filename);
		static void LoadPalette (std::string const& filename, std::array<int, 256>& palette);
	private:
	
		//MagicaVoxel .vox File Format [10/18/2016]
		//
		//Chunk 'MAIN'
		//{
		//	// pack of models
		//	Chunk 'PACK'    : optional
		//
		//	// models
		//	Chunk 'SIZE'
		//	Chunk 'XYZI'
		//
		//	Chunk 'SIZE'
		//	Chunk 'XYZI'
		//
		//	...
		//
		//	Chunk 'SIZE'
		//	Chunk 'XYZI'
		//
		//	// palette
		//	Chunk 'RGBA'    : optional
		//
		//	// materials
		//	Chunk 'MATT'    : optional
		//	Chunk 'MATT'
		//	...
		//	Chunk 'MATT'
		//}

		static void align (std::ifstream* const& file, size_t n);
		static bool fileHasValidSignature (std::string const& filename, std::ifstream* const& file);

		//1. File Structure : RIFF style
		//-------------------------------------------------------------------------------
		//# Bytes  | Type       | Value
		//-------------------------------------------------------------------------------
		//1x4      | char       | id 'VOX ' : 'V' 'O' 'X' 'space', 'V' is first
		//4        | int        | version number : 150

		static bool checkFileStructure (std::ifstream* const& file);

		//2. Chunk Structure
		//-------------------------------------------------------------------------------
		//# Bytes  | Type       | Value
		//-------------------------------------------------------------------------------
		//1x4      | char       | chunk id
		//4        | int        | num bytes of chunk content (N)
		//4        | int        | num bytes of children chunks (M)
		//
		//N        |            | chunk content
		//
		//M        |            | children chunks
		//-------------------------------------------------------------------------------

		enum VoxChunkId {
			NONE = 0, MAIN, PACK, SIZE, XYZI, RGBA, MATT
		};

		struct VoxChunk {
			VoxChunkId chunk_id;
			int size_chunk;
			int child_chunk;

			std::streamoff chunk_content;
		};

		static void parseChunk (std::ifstream* const& file, VoxChunk& c);
		static void parseVoxelite (std::ifstream* const& file, VoxChunk& c, Voxelite& data);

		/*
		3. Chunk id 'MAIN' : the root chunk and parent chunk of all the other chunks

		4. Chunk id 'PACK' : if it is absent, only one mesh in the file
		-------------------------------------------------------------------------------
		# Bytes  | Type       | Value
		-------------------------------------------------------------------------------
		4        | int        | numModels : num of SIZE and XYZI chunks
		-------------------------------------------------------------------------------

		5. Chunk id 'SIZE' : mesh size
		-------------------------------------------------------------------------------
		# Bytes  | Type       | Value
		-------------------------------------------------------------------------------
		4        | int        | size x
		4        | int        | size y
		4        | int        | size z : gravity direction
		-------------------------------------------------------------------------------
		6. Chunk id 'XYZI' : mesh voxels
		-------------------------------------------------------------------------------
		# Bytes  | Type       | Value
		-------------------------------------------------------------------------------
		4        | int        | numVoxels (N)
		4 x N    | int        | (x, y, z, colorIndex) : 1 byte for each component
		-------------------------------------------------------------------------------


		7. Chunk id 'RGBA' : palette
		-------------------------------------------------------------------------------
		# Bytes  | Type       | Value
		-------------------------------------------------------------------------------
		4 x 256  | int        | (R, G, B, A) : 1 byte for each component
							  | * <NOTICE>
							  | * color [0-254] are mapped to palette index [1-255], e.g :
							  |
							  | for ( int i = 0; i <= 254; i++ ) {
							  |     palette[i + 1] = ReadRGBA();
							  | }
		-------------------------------------------------------------------------------


		8. Default Palette : if chunk 'RGBA' is absent
		-------------------------------------------------------------------------------
		*/

		static constexpr unsigned int default_palette[256] = {
			0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
			0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
			0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
			0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
			0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
			0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
			0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
			0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
			0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
			0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
			0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
			0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
			0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
			0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
			0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
			0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
		};

		//9. Chunk id 'MATT' : material, if it is absent, it is diffuse material
		//-------------------------------------------------------------------------------
		//# Bytes  | Type     | Value
		//-------------------------------------------------------------------------------
		//4        | int      | id [1-255]
		//
		//4        | int      | material type
		//					  | 0 : diffuse
		//					  | 1 : metal
		//					  | 2 : glass
		//					  | 3 : emissive
		//
		//4        | float    | material weight
		//					  | diffuse  : 1.0
		//					  | metal    : (0.0 - 1.0] : blend between metal and diffuse material
		//					  | glass    : (0.0 - 1.0] : blend between glass and diffuse material
		//					  | emissive : (0.0 - 1.0] : self-illuminated material
		//
		//4        | int      | property bits : set if value is saved in next section
		//					  | bit(0) : Plastic
		//					  | bit(1) : Roughness
		//					  | bit(2) : Specular
		//					  | bit(3) : IOR
		//					  | bit(4) : Attenuation
		//					  | bit(5) : Power
		//					  | bit(6) : Glow
		//					  | bit(7) : isTotalPower (*no value)
		//
		//4 * N    | float    | normalized property value : (0.0 - 1.0]
		//					  | * need to map to real range
		//					  | * Plastic material only accepts {0.0, 1.0} for this version
		//-------------------------------------------------------------------------------
	
	};
}

/*
=================================
(a) STRING type

int32   : buffer size (in bytes)
int8xN	: buffer (without the ending "\0")

=================================
(b) DICT type

int32	: num of key-value pairs

// for each key-value pair
{
STRING	: key
STRING	: value
}xN

=================================
(c) ROTATION type

store a row-major rotation in the bits of a byte

for example :
R =
 0  1  0
 0  0 -1
-1  0  0 
==>
unsigned char _r = (1 << 0) | (2 << 2) | (0 << 4) | (1 << 5) | (1 << 6)

bit | value
0-1 : 1 : index of the non-zero entry in the first row
2-3 : 2 : index of the non-zero entry in the second row
4   : 0 : the sign in the first row (0 : positive; 1 : negative)
5   : 1 : the sign in the second row (0 : positive; 1 : negative)
6   : 1 : the sign in the third row (0 : positive; 1 : negative)

=================================
(d) Scene Graph

T : Transform Node
G : Group Node
S : Shape Node

     T
     |
     G
    / \
   T   T
   |   |
   G   S
  / \
 T   T
 |   |
 S   S

=================================
(1) Transform Node Chunk : "nTRN"

int32	: node id
DICT	: node attributes
	  (_name : string)
	  (_hidden : 0/1)
int32 	: child node id
int32 	: reserved id (must be -1)
int32	: layer id
int32	: num of frames (must be 1)

// for each frame
{
DICT	: frame attributes
	  (_r : int8) ROTATION, see (c)
	  (_t : int32x3) translation
}xN

=================================
(2) Group Node Chunk : "nGRP" 

int32	: node id
DICT	: node attributes
int32 	: num of children nodes

// for each child
{
int32	: child node id
}xN

=================================
(3) Shape Node Chunk : "nSHP" 

int32	: node id
DICT	: node attributes
int32 	: num of models (must be 1)

// for each mesh
{
int32	: mesh id
DICT	: mesh attributes : reserved
}xN

=================================
(4) Material Chunk : "MATL"

int32	: material id
DICT	: material properties
	  (_type : str) _diffuse, _metal, _glass, _emit
	  (_weight : float) range 0 ~ 1
	  (_rough : float)
	  (_spec : float)
	  (_ior : float)
	  (_att : float)
	  (_flux : float)
	  (_plastic)
	  
=================================
(5) Layer Chunk : "LAYR"
int32	: layer id
DICT	: layer atrribute
	  (_name : string)
	  (_hidden : 0/1)
int32	: reserved id, must be -1
*/
