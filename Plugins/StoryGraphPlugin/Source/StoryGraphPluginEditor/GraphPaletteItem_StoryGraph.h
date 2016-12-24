// Copyright 2016 Dmitriy Pavlov
#pragma once
#include "SGraphPalette.h"

class SGraphPaletteItem_StoryGraph : public SGraphPaletteItem
{

public:
	DECLARE_DELEGATE_TwoParams(FOnNameTextCommittedDelegate, const FText&, ETextCommit::Type);

	virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override;


	FOnNameTextCommittedDelegate OnNameTextCommittedDelegate;
};