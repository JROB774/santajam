GLOBAL constexpr float PLAYER_INPUT_REFRESH_TIME = 0.1f;
GLOBAL constexpr int PLAYER_MAX_ITEM_STACK = 999;

INTERNAL void PlayerPickUpItem (std::string name, int amount)
{
    if (amount <= 0) return;

    // If the specified base type can't be found then we don't pick up an item
    if (!gItems.count(name))
    {
        LOG_ERROR(ERR_MIN, "Could not pickup item of unknown type: %s", name.c_str());
        return;
    }

    // If we already have the item in our inventory then add to it.
    InventoryItem* found = NULL;
    for (auto& item: gPlayer.inventory.items)
    {
        if (item.name == name)
        {
            found = &item;
            item.amount += amount;
            break;
        }
    }
    // Otherwise we create a new entry for it.
    if (!found)
    {
        gPlayer.inventory.items.push_back({ name, amount });
        found = &gPlayer.inventory.items.back();
    }

    // Make sure the number of items does not exceed the stack limit.
    if (found->amount > PLAYER_MAX_ITEM_STACK)
    {
        found->amount = PLAYER_MAX_ITEM_STACK;
    }
}

INTERNAL void InitPlayer ()
{
    gPlayer.state = PLAYER_STATE_PLAY;

    gPlayer.input_timer = 0.0f;
    gPlayer.update = false;

    gPlayer.inventory.items.clear();
    gPlayer.inventory.selected_item = 0;

    gPlayer.inventory.bounds.current = { WINDOW_SCREEN_W/2,WINDOW_SCREEN_H/2, 0,0 };
    gPlayer.inventory.bounds.target = { WINDOW_SCREEN_W/2,WINDOW_SCREEN_H/2, 0,0 };

    // Set the inital position of the camera.
    Entity* p = MapGetFirstEntityOfType("player");
    if (p)
    {
        float cx = roundf(p->draw.pos.x + (TILE_W/2) - (WINDOW_SCREEN_W/2));
        float cy = roundf(p->draw.pos.y + (TILE_H/2) - (WINDOW_SCREEN_H/2));
        SetCamera(cx,cy);
    }
}

INTERNAL void UpdatePlayerStatePlay ()
{
    if (IsKeyPressed(SDL_SCANCODE_TAB))
    {
        gPlayer.state = PLAYER_STATE_INVENTORY;
        gPlayer.input_timer = 0.0f;
        gPlayer.inventory.bounds.current = { WINDOW_SCREEN_W/2,WINDOW_SCREEN_H/2, 0,0 };
        gPlayer.inventory.bounds.target = { TILE_W,TILE_H,WINDOW_SCREEN_W-(TILE_W*2),WINDOW_SCREEN_H-(TILE_H*2) };
        gPlayer.inventory.scale.current = { 0,0 };
        gPlayer.inventory.scale.target = { 1,1 };
        return;
    }

    // If any of the player's input keys are pressed we update the simulation.
    if (IsKeyPressed(SDL_SCANCODE_A) ||
        IsKeyPressed(SDL_SCANCODE_D) ||
        IsKeyPressed(SDL_SCANCODE_W) ||
        IsKeyPressed(SDL_SCANCODE_S) ||
        IsKeyPressed(SDL_SCANCODE_SPACE))
    {
        gPlayer.input_timer = PLAYER_INPUT_REFRESH_TIME * 3; // Initial cooldown is longer...
        gPlayer.update = true;
    }
    // This system exists so that keys can be held.
    if (IsKeyDown(SDL_SCANCODE_A) ||
        IsKeyDown(SDL_SCANCODE_D) ||
        IsKeyDown(SDL_SCANCODE_W) ||
        IsKeyDown(SDL_SCANCODE_S) ||
        IsKeyDown(SDL_SCANCODE_SPACE))
    {
        gPlayer.input_timer -= gApplication.delta_time;
        if (gPlayer.input_timer <= 0.0f)
        {
            gPlayer.input_timer = PLAYER_INPUT_REFRESH_TIME;
            gPlayer.update = true;
        }
    }
}

INTERNAL void UpdatePlayerStateInventory ()
{
    if (IsKeyPressed(SDL_SCANCODE_TAB))
    {
        gPlayer.state = PLAYER_STATE_PLAY;
        gPlayer.input_timer = 0.0f;
        gPlayer.inventory.bounds.target = { WINDOW_SCREEN_W/2,WINDOW_SCREEN_H/2, 0,0 };
        gPlayer.inventory.scale.target = { 0,0 };
        return;
    }

    bool update = false;

    // If any of the player's input keys are pressed we update the simulation.
    if (IsKeyPressed(SDL_SCANCODE_W)  ||
        IsKeyPressed(SDL_SCANCODE_S)  ||
        IsKeyPressed(SDL_SCANCODE_UP) ||
        IsKeyPressed(SDL_SCANCODE_DOWN))
    {
        gPlayer.input_timer = PLAYER_INPUT_REFRESH_TIME * 3; // Initial cooldown is longer.
        update = true;
    }
    // This system exists so that keys can be held.
    if (IsKeyDown(SDL_SCANCODE_W)  ||
        IsKeyDown(SDL_SCANCODE_S)  ||
        IsKeyDown(SDL_SCANCODE_UP) ||
        IsKeyDown(SDL_SCANCODE_DOWN))
    {
        gPlayer.input_timer -= gApplication.delta_time;
        if (gPlayer.input_timer <= 0.0f)
        {
            gPlayer.input_timer = PLAYER_INPUT_REFRESH_TIME;
            update = true;
        }
    }

    // INVENTORY
    if (update)
    {
        if (IsKeyDown(SDL_SCANCODE_W) || IsKeyDown(SDL_SCANCODE_UP)) gPlayer.inventory.selected_item--;
        if (IsKeyDown(SDL_SCANCODE_S) || IsKeyDown(SDL_SCANCODE_DOWN)) gPlayer.inventory.selected_item++;

        gPlayer.inventory.selected_item = std::clamp(gPlayer.inventory.selected_item, 0, (int)gPlayer.inventory.items.size()-1);
    }

    // CRAFTING
    // @Incomplete: ...
}

INTERNAL void UpdatePlayer ()
{
    gPlayer.update = false;

    // Update player based on the current state.
    switch (gPlayer.state)
    {
        case (PLAYER_STATE_PLAY): UpdatePlayerStatePlay(); break;
        case (PLAYER_STATE_INVENTORY): UpdatePlayerStateInventory(); break;
    }

    // The camera tracks the player's current position!
    Entity* p = MapGetFirstEntityOfType("player");
    if (p)
    {
        float cx = roundf(p->draw.pos.x + (TILE_W/2) - (WINDOW_SCREEN_W/2));
        float cy = roundf(p->draw.pos.y + (TILE_H/2) - (WINDOW_SCREEN_H/2));
        SetCameraTarget(cx,cy);
    }

    // Debug key for printing the player inventory.
    #if defined(BUILD_DEBUG)
    if (IsKeyPressed(SDL_SCANCODE_I))
    {
        printf("INVENTORY:\n");
        for (auto [item,amount]: gPlayer.inventory.items)
        {
            printf("  %d %s\n", amount, item.c_str());
        }
    }
    #endif // BUILD_DEBUG
}

INTERNAL void RenderPlayerInventory ()
{
    const Vec4 INVENTORY_BG_COLOR = SDLColorToColor({ 38,13,28,255 });
    const Vec4 INVENTORY_FG_COLOR = SDLColorToColor({ 244,180,27,255 });

    const float INVENTORY_INSET = 3;

    const float INVENTORY_ITEM_BOUNDS_W = 96;
    const float INVENTORY_ITEM_BOUNDS_H = 186;

    const float INVENTORY_TEXT_OFF = 1;

    gPlayer.inventory.bounds.current = Lerp(gPlayer.inventory.bounds.current, gPlayer.inventory.bounds.target, gApplication.delta_time*30);
    gPlayer.inventory.scale.current = Lerp(gPlayer.inventory.scale.current, gPlayer.inventory.scale.target, gApplication.delta_time*30);

    DrawFill(gPlayer.inventory.bounds.current, INVENTORY_BG_COLOR);

    // Don't draw if the inventory is closed...
    if (gPlayer.inventory.bounds.current.x >= ((WINDOW_SCREEN_W/2)-INVENTORY_INSET) &&
        gPlayer.inventory.bounds.current.y >= ((WINDOW_SCREEN_H/2)-INVENTORY_INSET))
    {
        return;
    }

    // Draw the actual inventory.

    float ix = gPlayer.inventory.bounds.current.x;
    float iy = gPlayer.inventory.bounds.current.y;

    DrawImage("inventorybg", ix,iy, gPlayer.inventory.scale.current, {0,0}, 0.0f, FLIP_NONE, INVENTORY_FG_COLOR);

    if ((gPlayer.inventory.bounds.current.w >= gPlayer.inventory.bounds.target.w-3) && (gPlayer.inventory.bounds.target.w != 0.0f))
    {
        // INVENTORY
        ScissorOn(27,35,127,88);
        {
            float x = 27;
            float y = 35;

            int index = 0;
            for (auto item: gPlayer.inventory.items)
            {
                if (item.amount > 0)
                {
                    Vec4 color = INVENTORY_FG_COLOR;
                    if (index == gPlayer.inventory.selected_item)
                    {
                        DrawFill(x,y,128,8, INVENTORY_FG_COLOR);
                        color = INVENTORY_BG_COLOR;
                    }

                    DrawImage("item", x+INVENTORY_TEXT_OFF,y, {0.5f,0.5f}, {0,0}, 0.0f, FLIP_NONE, color, &GetItem(item.name).clip);
                    DrawText("main", StrUpper(item.name), x+INVENTORY_TEXT_OFF+12,y, color);
                    std::string quant = std::to_string(item.amount);
                    DrawText("main", StrUpper(quant), (x+127)-(INVENTORY_TEXT_OFF+GetTextWidth("main",quant)),y, color);

                    y += 8;
                    index++;
                }
            }
        }
        ScissorOff();

        // CRAFTING
        ScissorOn(166,35,127,88);
        {
            // @Incomplete: ...
        }
        ScissorOff();
    }

    DrawImage("inventoryfg", ix,iy, gPlayer.inventory.scale.current, {0,0}, 0.0f, FLIP_NONE, INVENTORY_FG_COLOR);
}

INTERNAL void RenderPlayer ()
{
    RenderPlayerInventory();
}
