#include "../Misc/pch.h"

#include "Chunk.h"

using namespace DirectX;

Chunk::Chunk() : m_active(true), m_id(0), m_pos(XMFLOAT3(-5.0f, 0, 0))
{
	// Ensures that a chunk will ALWAYS generate it's corresponding blocks
	InitializeChunk();
	InitializeBuffers();
}

Chunk::~Chunk()
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				Block* block = m_chunk[x][y][z];
				delete block;
			}
		}
	}
}

const uint32_t Chunk::GetID() { return m_id; }

const Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return m_chunk[x][y][z]; }

void Chunk::SetActive(const bool active) { m_active = active; }

const bool Chunk::GetActive() { return m_active; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const DirectX::XMFLOAT3* Chunk::GetBlockPositions() { return &m_blockPositions[0]; }

void Chunk::InitializeChunk()
{
	m_active = true;
	//m_id = 0; // TODO: Replace with an actual UUID
	//m_pos = XMFLOAT3(0, 0, 0) // TODO: Relate the chunk ID with it's position

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				Block* newBlock = new Block(BlockType::Dirt);
				m_chunk[x][y][z] = newBlock;
			}
		}
	}
}

void Chunk::InitializeBuffers()
{
	uint16_t index = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				XMFLOAT3 blockPos = 
				{ static_cast<float>(x) + m_pos.x, 
				  static_cast<float>(y) + m_pos.y, 
				  static_cast<float>(z) + m_pos.z
				};

				// It would be good to only render blocks that are not occluded
				m_blockPositions[index++] = blockPos;

			}
		}
	}
}
