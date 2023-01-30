#include "MemeField.h"
#include <assert.h>
#include<random>
#include "Vei2.h"
#include "SpriteCodex.h"
#include<algorithm>


void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2& screenPos, bool fucked, Graphics& gfx) const
{
	if (!fucked) {
		switch (state)
		{
		case State::HIdden:

			SpriteCodex::DrawTileButton(screenPos, gfx);

			break;
		case State::Flagged:
			if (HasMeme())

				SpriteCodex::DrawTileFlag(screenPos, gfx);

			break;
		case State::Revealed:

			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;



		}
	}
	else
	{
		switch (state)
		{
		case State::HIdden:
			
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flagged:
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);

			}

			break;
		case State::Revealed:

			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;

		}

	}
}

void MemeField::Tile::Reveal()
{
	assert(state == State::HIdden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}
void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	if (state == State::HIdden)
	{
		state = State::Flagged;
	}
	else
	{
		state = State::HIdden;
	}
}
bool MemeField::Tile::IsFlagged() const
{
	return state==State::Flagged;
}
void MemeField::Tile::SetNeibhourMemeCount(int memeCount)
{
	assert(nNeighborMemes == -1);
	nNeighborMemes = memeCount;
}
MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width* height - 1);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width  - 1);
	std::uniform_int_distribution<int> yDist(0,  height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; nSpawned++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng),yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());
		TileAt(spawnPos).SpawnMeme();
	}

/*	for (int i = 0; i < 120; i++)
	{
		Vei2 gridPos = { xDist(rng),yDist(rng) };
		if (!TileAt(gridPos).IsRevealed())
		TileAt(gridPos).Reveal();
	}*/
	
	for (Vei2 gridpos = { 0,0 }; gridpos.y <height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x <width; gridpos.x++)
		{
			TileAt(gridpos).SetNeibhourMemeCount(CountNeibhorMemes(gridpos));
		}
	}

}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x=0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw(gridPos*SpriteCodex::tileSize,isFucked,gfx);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0, width * SpriteCodex::tileSize, 0, height*SpriteCodex::tileSize);
}

void MemeField::OnRevelClick(const Vei2& screenpos)
{
	if (!isFucked)
	{
		const Vei2 gridPos = ScreenToGrid(screenpos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged())
		{
			tile.Reveal();
			if (tile.HasMeme())
			{
				isFucked = true;
			}
			
		}
	}

}

void MemeField::OnFlagClick(const Vei2& screenpos)
{
	if (!isFucked)
	{
		const Vei2 gridPos = ScreenToGrid(screenpos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.ToggleFlag();
		}
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridPos)
{
	return field[(gridPos.y * width + gridPos.x)];
}

const MemeField::Tile& MemeField::TileAt(const Vei2& gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MemeField::ScreenToGrid(const Vei2& screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}

int MemeField::CountNeibhorMemes(const Vei2& gridpos)
{
	int count = 0;
	const int xStart = std::max(0, gridpos.x - 1);
	const int yStart = std::max(0, gridpos.y - 1);
	const int xEnd = std::min(width - 1, gridpos.x + 1);
	const int yEnd = std::min(width - 1, gridpos.y + 1);
	for (Vei2 gridpos = { xStart,yStart }; gridpos.y <= yEnd; gridpos.y++)
	{
		for (gridpos.x = xStart; gridpos.x <= xEnd; gridpos.x++)
		{
			if (TileAt(gridpos).HasMeme())
			{
				count++;
			}
		}
	}
	return count;
}
