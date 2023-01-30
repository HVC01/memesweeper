#pragma once
#include"Graphics.h"
class MemeField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			HIdden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMeme();
		bool HasMeme()const;
		void Draw(const Vei2& screenPos,bool fucked,Graphics& gfx)const;
		void Reveal();
		bool IsRevealed()const;
		void ToggleFlag();
		bool IsFlagged()const;
		void SetNeibhourMemeCount(int memeCount);
	private:
		State state = State::HIdden;
		bool hasMeme = false;
		int nNeighborMemes = -1;

	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx)const;
	RectI GetRect()const;
	void OnRevelClick(const Vei2& screenpos);
	void OnFlagClick(const Vei2& screenpos);

private:
	Tile& TileAt(const Vei2& gripdPos);
	const Tile& TileAt(const Vei2& gridPos)const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
	int CountNeibhorMemes(const Vei2& gridpos);

private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	bool isFucked = false;
	Tile field[width * height];

};