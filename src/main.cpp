#include "BlockType.h"
#include "Chunk.h"
#include "Coord.h"
#include "Pixel.h"
#include "World.h"
#include <cassert>
#include <iostream>
#include <unordered_map>

// THIS enables colored output on Windows terminal
#ifdef _WIN32
#include <windows.h>
inline void enable_virtual_terminal() {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hOut, &mode);
  SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

using namespace std;

void test_coord() {
  cout << "=== COORD TESTS ===\n";

  // 1. Construction
  Coord a = {3, 4};
  Coord b = {1, 2};
  Coord origin; // should be (0, 0) from defaults
  cout << "a = " << a << ", b = " << b << ", origin = " << origin << "\n";

  // 2. Arithmetic
  Coord sum = a + b;
  Coord diff = a - b;
  cout << a << " + " << b << " = " << sum << "\n";
  cout << a << " - " << b << " = " << diff << "\n";
  assert(sum.x == 4 && sum.y == 6);
  assert(diff.x == 2 && diff.y == 2);

  // 3. Equality
  Coord a_copy = {3, 4};
  assert(a == a_copy);
  assert(!(a == b));
  cout << a << " == " << a_copy << " : true\n";

  // 4. HashMap with CoordHash
  unordered_map<Coord, string, CoordHash> map;
  map[{0, 0}] = "Origin";
  map[{10, 5}] = "Chunk_1_0";
  map[{-5, 3}] = "Negative";
  auto val1 = map[{0, 0}];
  assert(val1 == "Origin");
  auto val2 = map[{10, 5}];
  assert(val2 == "Chunk_1_0");
  auto val3 = map.count({99, 99});
  assert(val3 == 0);
  cout << "HashMap: 3 inserted, lookup works, missing key returns 0\n";

  cout << "All Coord tests PASSED!\n\n";
}

void test_blocktype() {
  cout << "=== BLOCKTYPE TESTS ===\n";

  // 1. Size check — must be 1 byte
  assert(sizeof(BlockType) == 1);
  cout << "Size: " << sizeof(BlockType) << " byte (uint8_t confirmed)\n";

  // 2. COUNT value
  assert(static_cast<int>(BlockType::COUNT) == 8);
  cout << "Total types: " << static_cast<int>(BlockType::COUNT) << "\n";

  // 3. Char mapping
  assert(block_to_char(BlockType::STONE) == '#');
  assert(block_to_char(BlockType::DIAMOND) == 'D');
  assert(block_to_char(BlockType::AIR) == ' ');
  cout << "Char mapping: correct\n";

  // 4. String mapping
  assert(block_to_string(BlockType::GOLD) == "Gold");
  assert(block_to_string(BlockType::GRASS) == "Grass");
  cout << "String mapping: correct\n";

  // 5. Print all types
  for (int i = 0; i < static_cast<int>(BlockType::COUNT); i++) {
    BlockType b = static_cast<BlockType>(i);
    cout << "  [" << block_to_char(b) << "] " << block_to_string(b) << "\n";
  }

  cout << "All BlockType tests PASSED!\n\n";
}

void test_pixel() {
  cout << "=== PIXEL TESTS ===\n";

  // 1. Size check — must be 2 bytes
  assert(sizeof(Pixel) == 2);
  cout << "Size: " << sizeof(Pixel) << " bytes (char + Color)\n";

  // 2. Default pixel is white space
  Pixel empty;
  assert(empty.ch == ' ');
  assert(empty.color == Color::WHITE);
  cout << "Default pixel: white space (transparent) confirmed\n";

  // 3. block_to_pixel mapping
  Pixel grass = block_to_pixel(BlockType::GRASS);
  assert(grass.ch == '"');
  assert(grass.color == Color::GREEN);

  Pixel diamond = block_to_pixel(BlockType::DIAMOND);
  assert(diamond.ch == 'D');
  assert(diamond.color == Color::CYAN);
  cout << "block_to_pixel mapping: correct\n";

  // 4. VISUAL TEST — colored output!
  cout << "\nColored block display:\n";
  for (int i = 0; i < static_cast<int>(BlockType::COUNT); i++) {
    BlockType b = static_cast<BlockType>(i);
    Pixel p = block_to_pixel(b);
    cout << "  " << p << "  " << block_to_string(b) << "\n";
  }

  // 5. Mini world preview — a row of blocks!
  cout << "\nMini terrain row: ";
  BlockType row[] = {BlockType::AIR,    BlockType::AIR,   BlockType::GRASS,
                     BlockType::DIRT,   BlockType::STONE, BlockType::STONE,
                     BlockType::IRON,   BlockType::GOLD,  BlockType::DIAMOND,
                     BlockType::BEDROCK};
  for (auto b : row) {
    cout << block_to_pixel(b);
  }
  cout << "\n";

  cout << "All Pixel tests PASSED!\n\n";
}

// INTEGRATION: All three types working together
void test_integration() {
  cout << "=== INTEGRATION TEST ===\n";

  // Simulate: "What block is at world position (25, 7)?"
  Coord world_pos = {25, 7};

  // Which chunk is this in? (chunk size = 10)
  Coord chunk_pos = {world_pos.x / 10, world_pos.y / 10};

  // What's the local position within the chunk?
  Coord local_pos = {world_pos.x % 10, world_pos.y % 10};

  cout << "World pos: " << world_pos << "\n";
  cout << "Chunk pos: " << chunk_pos << " (chunk 2, 0)\n";
  cout << "Local pos: " << local_pos << " (cell 5, 7)\n";

  // Pretend this block is diamond
  BlockType block = BlockType::DIAMOND;
  Pixel visual = block_to_pixel(block);

  cout << "Block at " << world_pos << " is " << block_to_string(block) << " -> "
       << visual << "\n";

  // Store in a world map
  unordered_map<Coord, BlockType, CoordHash> world;
  world[world_pos] = BlockType::DIAMOND;
  world[{0, 0}] = BlockType::GRASS;
  world[{0, 5}] = BlockType::STONE;

  cout << "World map has " << world.size() << " blocks stored\n";

  // Retrieve and display
  for (auto &[pos, type] : world) {
    cout << "  " << pos << " : " << block_to_pixel(type) << " "
         << block_to_string(type) << "\n";
  }

  cout << "Integration test PASSED!\n";
}

void test_chunk() {
  cout << "\n=== CHUNK TESTS ===\n";

  // 1. Create a chunk at position (0, 0)
  Chunk chunk({0, 0});
  print_chunk(chunk);

  // 2. Verify terrain layers
  assert(chunk.get_block(5, 0) == BlockType::AIR);
  assert(chunk.get_block(5, 3) == BlockType::GRASS);
  assert(chunk.get_block(5, 5) == BlockType::DIRT);
  assert(chunk.get_block(5, 7) == BlockType::STONE);
  assert(chunk.get_block(5, 9) == BlockType::BEDROCK);
  cout << "Terrain layers: correct\n";

  // 3. Out-of-bounds returns AIR
  assert(chunk.get_block(-1, 0) == BlockType::AIR);
  assert(chunk.get_block(99, 99) == BlockType::AIR);
  cout << "Out-of-bounds safety: correct\n";

  // 4. Mining! Change a stone to air
  chunk.set_block(5, 7, BlockType::AIR);
  assert(chunk.get_block(5, 7) == BlockType::AIR);
  cout << "Mining (set_block): correct\n";

  // 5. Building! Place diamond where air was
  chunk.set_block(5, 1, BlockType::DIAMOND);
  assert(chunk.get_block(5, 1) == BlockType::DIAMOND);
  cout << "Building (set_block): correct\n";

  // 6. Print modified chunk
  cout << "\nAfter mining + building:\n";
  print_chunk(chunk);

  cout << "All Chunk tests PASSED!\n";
}

void test_world() {
  cout << "\n=== WORLD TESTS ===\n";

  World world;

  // 1. World starts empty
  assert(world.chunk_count() == 0);
  cout << "Empty world: 0 chunks\n";

  // 2. Access a block — chunk auto-created (lazy loading!)
  BlockType b = world.get_block(25, 7);
  cout << "Block at (25, 7): " << block_to_string(b) << "\n";
  assert(world.chunk_count() == 1);
  cout << "After first access: " << world.chunk_count() << " chunk loaded\n";

  // 3. Access same chunk — no new chunk created
  world.get_block(20, 3); // still in chunk (2, 0)
  assert(world.chunk_count() == 1);
  cout << "Same chunk access: still " << world.chunk_count() << " chunk\n";

  // 4. Access different chunk — new chunk auto-created
  world.get_block(35, 7); // chunk (3, 0)
  assert(world.chunk_count() == 2);
  cout << "Different chunk: " << world.chunk_count() << " chunks now\n";

  // 5. Negative coordinates work
  world.get_block(-5, -15);
  assert(world.chunk_count() == 3);
  cout << "Negative coords: " << world.chunk_count() << " chunks\n";

  // 6. Mining in world coordinates
  world.set_block(25, 7, BlockType::AIR);
  assert(world.get_block(25, 7) == BlockType::AIR);
  cout << "Mining at (25,7): correct\n";

  // 7. Print a slice of the world (3 chunks wide)
  cout << "\nWorld view (x: 0-29, y: 0-9):\n";
  print_world(world, 0, 29, 0, 9);

  cout << "All World tests PASSED!\n";
}

int main() {
#ifdef _WIN32
  enable_virtual_terminal();
#endif

  test_coord();
  test_blocktype();
  test_pixel();
  test_integration();
  test_chunk();
  test_world();

  return 0;
}