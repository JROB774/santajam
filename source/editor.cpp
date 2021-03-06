GLOBAL constexpr float EDITOR_PANEL_BORDER             =   4.0f;
GLOBAL constexpr float EDITOR_PALETTE_WIDTH            = 308.0f;
GLOBAL constexpr float EDITOR_PALETTE_ICON_SCALE       =   2.0f;
GLOBAL constexpr float EDITOR_PALETTE_ICON_PADDING     =   5.0f;
GLOBAL constexpr float EDITOR_PALETTE_ICON_HOVER_SCALE =   2.4f;
GLOBAL constexpr float EDITOR_PALETTE_ICON_HIT_SCALE   =   3.0f;
GLOBAL constexpr float EDITOR_PALETTE_ICON_HIT_ANGLE   = -30.0f;
GLOBAL constexpr float EDITOR_CONTROLS_HEIGHT          =  40.0f;
GLOBAL constexpr float EDITOR_CONTROLS_PADDING         =   4.0f;
GLOBAL constexpr float EDITOR_CURSOR_ALPHA             =   0.5f;
GLOBAL constexpr float EDITOR_THING_PLACE_SCALE        =   1.5f;
GLOBAL constexpr float EDITOR_THING_PLACE_ANGLE        = -30.0f;
GLOBAL constexpr float EDITOR_THING_ERASE_SCALE        =   1.5f;
GLOBAL constexpr float EDITOR_THING_ERASE_ANGLE        = -30.0f;
GLOBAL constexpr Vec4  EDITOR_COLOR0                   = { 0.28f,0.18f,0.24f,1.0f };
GLOBAL constexpr Vec4  EDITOR_COLOR1                   = { 0.81f,0.77f,0.72f,1.0f };
GLOBAL constexpr Vec4  EDITOR_COLOR2                   = { 1.00f,1.00f,1.00f,1.0f };

struct EditorIcon
{
    std::string type;

    // Drawing stuff.
    struct
    {
        SDL_Rect clip;

        struct
        {
            float current;
            float target;
        } scale;
        struct
        {
            float current;
            float target;
        } angle;
        struct
        {
            Vec4 current;
            Vec4 target;
            Vec4 base;
        } color;
    } draw;
};

struct EditorIconIndex
{
    std::string type; // "tile" or "entity"
    int index;

    inline bool operator== (const EditorIconIndex& o)
    {
        return (type == o.type && index == o.index);
    }
    inline bool operator!= (const EditorIconIndex& o)
    {
        return !(operator==(o));
    }
};

struct EditorThing
{
    EditorIconIndex icon_index;

    bool active;

    // Drawing stuff.
    struct
    {
        SDL_Rect clip;

        struct
        {
            float current;
            float target;
        } scale;
        struct
        {
            float current;
            float target;
        } angle;
        struct
        {
            Vec4 current;
            Vec4 target;
            Vec4 base;
        } color;
    } draw;
};

struct EditorChunk
{
    EditorThing things[CHUNK_H][CHUNK_W];
};

GLOBAL struct Editor
{
    std::vector<EditorIcon> tile_icons;
    std::vector<EditorIcon> entity_icons;

    struct
    {
        std::string name;
        std::vector<EditorChunk> chunks;
        size_t current;
    } zone;

    struct
    {
        Rect bounds;
    } palette;
    struct
    {
        Rect bounds;
    } controls;
    struct
    {
        Rect space;
        Rect bounds;
        float scale;
    } canvas;

    struct
    {
        struct { int x,y; } pos;

        struct
        {
            Vec2 pos;
            struct
            {
                Vec4 current;
                Vec4 target;
            } color;
        } draw;

        EditorIconIndex selected;
    } cursor;

} gEditor;

INTERNAL EditorIcon* GetEditorIconFromIndex (EditorIconIndex index)
{
    EditorIcon* icon = NULL;
    if (index.type == "tile")
    {
        if (!gEditor.tile_icons.empty())
        {
            icon = &gEditor.tile_icons.at(index.index);
        }
    }
    else if (index.type == "entity")
    {
        if (!gEditor.entity_icons.empty())
        {
            icon = &gEditor.entity_icons.at(index.index);
        }
    }
    return icon;
}

INTERNAL EditorIcon* GetSelectedEditorIcon ()
{
    return GetEditorIconFromIndex(gEditor.cursor.selected);
}

INTERNAL EditorChunk& GetCurrentEditorChunk ()
{
    return gEditor.zone.chunks.at(gEditor.zone.current);
}

INTERNAL EditorIconIndex GetEditorIconIndex (std::string type, std::string name)
{
    EditorIconIndex index;
    index.type = type;

    if (type == "tile")
    {
        for (int i=0; i<gEditor.tile_icons.size(); ++i)
        {
            if (name == gEditor.tile_icons.at(i).type)
            {
                index.index = i;
                break;
            }
        }
    }
    else if (type == "entity")
    {
        for (int i=0; i<gEditor.entity_icons.size(); ++i)
        {
            if (name == gEditor.entity_icons.at(i).type)
            {
                index.index = i;
                break;
            }
        }
    }

    return index;
}

INTERNAL bool IsEditorChunkEmpty (EditorChunk& chunk)
{
    for (int iy=0; iy<CHUNK_H; ++iy)
    {
        for (int ix=0; ix<CHUNK_W; ++ix)
        {
            if (chunk.things[iy][ix].active)
            {
                return false;
            }
        }
    }
    return true;
}

INTERNAL void EditorPlaceThing (EditorThing& thing, EditorIconIndex index)
{
    EditorIcon* icon = GetEditorIconFromIndex(index);
    if (icon)
    {
        // Only place something is nothing is there or the tile/entity types are different.
        if (!thing.active || (gEditor.cursor.selected != thing.icon_index))
        {
            thing.icon_index = index;
            thing.active = true;

            thing.draw.clip = icon->draw.clip;
            thing.draw.scale.current = EDITOR_THING_PLACE_SCALE;
            thing.draw.scale.target = 1.0f;
            thing.draw.angle.current = EDITOR_THING_PLACE_ANGLE;
            thing.draw.angle.target = 0.0f;
            thing.draw.color.current = EDITOR_COLOR2;
            thing.draw.color.target = icon->draw.color.base;
            thing.draw.color.base = icon->draw.color.base;
        }
    }
}

INTERNAL void EditorEraseThing (EditorThing& thing)
{
    if (thing.active)
    {
        thing.active = false;

        thing.draw.scale.current = EDITOR_THING_ERASE_SCALE;
        thing.draw.scale.target = 0.0f;
        thing.draw.angle.current = EDITOR_THING_ERASE_ANGLE;
        thing.draw.angle.target = 0.0f;
    }
}

INTERNAL void EditorIncrementZoneChunk ()
{
    if (gEditor.zone.current < gEditor.zone.chunks.size()-1) gEditor.zone.current++;
}

INTERNAL void EditorDecrementZoneChunk ()
{
    if (gEditor.zone.current > 0) gEditor.zone.current--;
}

INTERNAL void EditorNewZoneChunk ()
{
    gEditor.zone.chunks.push_back({});
    gEditor.zone.current = gEditor.zone.chunks.size()-1;
}

INTERNAL void EditorClearZoneChunk ()
{
    EditorChunk& chunk = GetCurrentEditorChunk();
    for (int iy=0; iy<CHUNK_H; ++iy)
    {
        for (int ix=0; ix<CHUNK_W; ++ix)
        {
            EditorEraseThing(GetCurrentEditorChunk().things[iy][ix]);
        }
    }
}

INTERNAL void EditorLoadZoneChunks ()
{
    gEditor.zone.chunks.clear();

    std::string file_name = gAssetManager.asset_base_path + AssetData::Path + "chunks/" + gEditor.zone.name + AssetData::Ext;
    if (std::filesystem::exists(file_name))
    {
        GonObject gon = GonObject::Load(file_name);
        if (gon.type == GonObject::FieldType::OBJECT)
        {
            if (gon.Contains("chunks"))
            {
                // Go through and load each chunk.
                for (auto& data: gon["chunks"].children_array)
                {
                    gEditor.zone.chunks.push_back({});
                    EditorChunk& chunk = gEditor.zone.chunks.back();

                    // Tiles
                    if (data.Contains("tiles"))
                    {
                        for (auto& tile: data["tiles"].children_array)
                        {
                            int x = tile[0].Int(), y = tile[1].Int();
                            std::string name = tile[2].String();
                            EditorThing& thing = chunk.things[y][x];
                            EditorPlaceThing(thing, GetEditorIconIndex("tile", name));
                        }
                    }
                    // Entities
                    if (data.Contains("entities"))
                    {
                        for (auto& entity: data["entities"].children_array)
                        {
                            int x = entity[0].Int(), y = entity[1].Int();
                            std::string name = entity[2].String();
                            EditorThing& thing = chunk.things[y][x];
                            EditorPlaceThing(thing, GetEditorIconIndex("entity", name));
                        }
                    }
                }
            }
        }
    }

    // If there were no chunks found or loaded, just add an empty chunk to the end.
    // Otherwise just set our current chunk to the last one that was loaded from disk.
    if (gEditor.zone.chunks.empty()) EditorNewZoneChunk();
    else gEditor.zone.current = gEditor.zone.chunks.size()-1;
}

INTERNAL void EditorSaveZoneChunks ()
{
    std::string file_name = gAssetManager.asset_base_path + AssetData::Path + "chunks/" + gEditor.zone.name + AssetData::Ext;

    std::ofstream file(file_name);
    if (!file.is_open())
    {
        LOG_ERROR(ERR_MED, "Failed to save editor chunks!");
    }
    else
    {
        // Go through and convert each chunk to GON data for saving.
        std::string data;
        data += "chunks\n[";
        for (auto& chunk: gEditor.zone.chunks)
        {
            if (!IsEditorChunkEmpty(chunk)) // Only save non-empty chunks.
            {
                data += "\n";

                std::string tiles;
                std::string entities;

                for (int iy=0; iy<CHUNK_H; ++iy)
                {
                    for (int ix=0; ix<CHUNK_W; ++ix)
                    {
                        EditorThing& thing = chunk.things[iy][ix];
                        if (thing.active)
                        {
                            EditorIcon* icon = GetEditorIconFromIndex(thing.icon_index);
                            if (icon)
                            {
                                if (thing.icon_index.type == "tile")
                                {
                                    tiles += "            [ " + std::to_string(ix) + " " + std::to_string(iy) + " " + icon->type + " ]\n";
                                }
                                else if (thing.icon_index.type == "entity")
                                {
                                    entities += "            [ " + std::to_string(ix) + " " + std::to_string(iy) + " " + icon->type + " ]\n";
                                }
                            }
                        }
                    }
                }

                data += "    {\n";
                data += "        tiles\n        [\n" + tiles +  "        ]\n";
                data += "        entities\n        [\n" + entities +  "        ]\n";
                data += "    }\n";
            }
        }
        data += "]\n";

        file << data;
    }
}

INTERNAL void InitEditor ()
{
    // Cache information on all the tiles and entities for the ditor.
    for (auto& [k,t]: gTiles)
    {
        EditorIcon icon;
        icon.type = k;
        icon.draw.clip = { t.image.x*TILE_W, t.image.y*TILE_H, TILE_W, TILE_H };
        icon.draw.scale.current = EDITOR_PALETTE_ICON_SCALE;
        icon.draw.scale.target = EDITOR_PALETTE_ICON_SCALE;
        icon.draw.angle.current = 0.0f;
        icon.draw.angle.target = 0.0f;
        icon.draw.color.current = t.color;
        icon.draw.color.target = t.color;
        icon.draw.color.base = t.color;
        gEditor.tile_icons.push_back(icon);
    }
    for (auto& [k,e]: gEntities)
    {
        EditorIcon icon;
        icon.type = k;
        icon.draw.clip = { e.image.x*TILE_W, e.image.y*TILE_H, TILE_W, TILE_H };
        icon.draw.scale.current = EDITOR_PALETTE_ICON_SCALE;
        icon.draw.scale.target = EDITOR_PALETTE_ICON_SCALE;
        icon.draw.angle.current = 0.0f;
        icon.draw.angle.target = 0.0f;
        icon.draw.color.current = e.color;
        icon.draw.color.target = e.color;
        icon.draw.color.base = e.color;
        gEditor.entity_icons.push_back(icon);
    }

    // Load all the chunks for the current zone into the editor.
    gEditor.zone.name = "overworld";
    EditorLoadZoneChunks();

    // Set the currently selected item to be the first tile.
    gEditor.cursor.selected.type = "tile";
    gEditor.cursor.selected.index = 0;

    gEditor.cursor.pos.x = 0;
    gEditor.cursor.pos.y = 0;
    gEditor.cursor.draw.pos.x = 0.0f;
    gEditor.cursor.draw.pos.y = 0.0f;
    gEditor.cursor.draw.color.current = GetSelectedEditorIcon()->draw.color.base;
    gEditor.cursor.draw.color.target = GetSelectedEditorIcon()->draw.color.base;
    gEditor.cursor.draw.color.current.a = EDITOR_CURSOR_ALPHA;
    gEditor.cursor.draw.color.target.a = EDITOR_CURSOR_ALPHA;
}

INTERNAL void QuitEditor ()
{
    if (gApplication.editor) EditorSaveZoneChunks();
}

INTERNAL void DoEditorPaletteIcons (float& cx, float& cy, std::vector<EditorIcon>& icons, std::string image)
{
    constexpr float ICON_W = TILE_W * EDITOR_PALETTE_ICON_SCALE;
    constexpr float ICON_H = TILE_H * EDITOR_PALETTE_ICON_SCALE;

    Vec2 mouse = GetMousePos();

    for (int i=0; i<(int)icons.size(); ++i)
    {
        EditorIcon& t = icons.at(i);

        if (cx+ICON_W > gEditor.palette.bounds.w)
        {
            cx = EDITOR_PALETTE_ICON_PADDING;
            cy += ICON_H + EDITOR_PALETTE_ICON_PADDING;
        }

        float x = gEditor.palette.bounds.x + cx;
        float y = gEditor.palette.bounds.y + cy;

        Rect bounds = { x,y,ICON_W,ICON_H };

        // If the mouse is hovering over the editor icon then apply some feedback effects.
        // We do this before applying scale so the bounding box doesn't get messed with.
        if (PointAndRectCollision(mouse, bounds))
        {
            t.draw.scale.target = EDITOR_PALETTE_ICON_HOVER_SCALE;

            if (IsMouseButtonPressed(SDL_BUTTON_LEFT))
            {
                t.draw.scale.current = EDITOR_PALETTE_ICON_HIT_SCALE;
                t.draw.angle.current = EDITOR_PALETTE_ICON_HIT_ANGLE;
                t.draw.color.current = EDITOR_COLOR2;

                // Set the new selected!
                gEditor.cursor.selected.type = image;
                gEditor.cursor.selected.index = i;

                float ca = gEditor.cursor.draw.color.current.a;
                float ta = gEditor.cursor.draw.color.target.a;

                gEditor.cursor.draw.color.current = GetSelectedEditorIcon()->draw.color.base;
                gEditor.cursor.draw.color.target = GetSelectedEditorIcon()->draw.color.base;
                gEditor.cursor.draw.color.current.a = ca;
                gEditor.cursor.draw.color.target.a = ta;
            }
        }
        else
        {
            t.draw.color.target = t.draw.color.base;
            t.draw.scale.target = EDITOR_PALETTE_ICON_SCALE;
        }

        // Apply lerping effects.
        t.draw.color.current = Lerp(t.draw.color.current, t.draw.color.target, gApplication.delta_time*15);
        t.draw.scale.current = Lerp(t.draw.scale.current, t.draw.scale.target, gApplication.delta_time*30);
        t.draw.angle.current = Lerp(t.draw.angle.current, t.draw.angle.target, gApplication.delta_time*15);

        // Adjust the X and Y for the current scale of the icon so it remains centered.
        x -= (TILE_W * (t.draw.scale.current - EDITOR_PALETTE_ICON_SCALE)) / 2;
        y -= (TILE_H * (t.draw.scale.current - EDITOR_PALETTE_ICON_SCALE)) / 2;

        Vec2 scale = { t.draw.scale.current, t.draw.scale.current };
        Vec2 center = { (TILE_W*scale.x)/2, (TILE_H*scale.y)/2 };
        DrawImage(image, x,y, scale, center, t.draw.angle.current, FLIP_NONE, t.draw.color.current, &t.draw.clip);
        cx += ICON_W + EDITOR_PALETTE_ICON_PADDING;
    }
}

INTERNAL void DoEditorPalette ()
{
    float ww = (float)GetWindowWidth();
    float wh = (float)GetWindowHeight();

    gEditor.palette.bounds.x = ww - (EDITOR_PALETTE_WIDTH + EDITOR_PANEL_BORDER);
    gEditor.palette.bounds.y = EDITOR_PANEL_BORDER;
    gEditor.palette.bounds.w = EDITOR_PALETTE_WIDTH;
    gEditor.palette.bounds.h = wh - (EDITOR_PANEL_BORDER * 2);

    DrawRect(gEditor.palette.bounds, EDITOR_COLOR1);

    // List all of the placeable tiles and entities.
    float cx = EDITOR_PALETTE_ICON_PADDING, cy = EDITOR_PALETTE_ICON_PADDING;
    DoEditorPaletteIcons(cx,cy, gEditor.tile_icons, "tile");
    cx = EDITOR_PALETTE_ICON_PADDING, cy += (TILE_H*EDITOR_PALETTE_ICON_SCALE) + (EDITOR_PALETTE_ICON_PADDING*5);
    DoEditorPaletteIcons(cx,cy, gEditor.entity_icons, "entity");

    // Based on the mouse scroll, change the currently selected thing.
    int scroll = GetMouseScrollVertical();
    if (scroll != 0)
    {
        if (scroll > 0) gEditor.cursor.selected.index++;
        else if (scroll < 0) gEditor.cursor.selected.index--;

        // If we have gone out of bounds then wrap around!
        if (gEditor.cursor.selected.type == "tile")
        {
            if (gEditor.cursor.selected.index < 0)
            {
                gEditor.cursor.selected.type = "entity";
                gEditor.cursor.selected.index = (int)gEditor.entity_icons.size()-1;
            }
            else if (gEditor.cursor.selected.index >= gEditor.tile_icons.size())
            {
                gEditor.cursor.selected.type = "entity";
                gEditor.cursor.selected.index = 0;
            }
        }
        else if (gEditor.cursor.selected.type == "entity")
        {
            if (gEditor.cursor.selected.index < 0)
            {
                gEditor.cursor.selected.type = "tile";
                gEditor.cursor.selected.index = (int)gEditor.tile_icons.size()-1;
            }
            else if (gEditor.cursor.selected.index >= gEditor.entity_icons.size())
            {
                gEditor.cursor.selected.type = "tile";
                gEditor.cursor.selected.index = 0;
            }
        }

        // We need to update the color!
        float ca = gEditor.cursor.draw.color.current.a;
        float ta = gEditor.cursor.draw.color.target.a;
        gEditor.cursor.draw.color.current = GetSelectedEditorIcon()->draw.color.base;
        gEditor.cursor.draw.color.target = GetSelectedEditorIcon()->draw.color.base;
        gEditor.cursor.draw.color.current.a = ca;
        gEditor.cursor.draw.color.target.a = ta;
    }
}

INTERNAL void DoEditorControls ()
{
    float ww = (float)GetWindowWidth();
    float wh = (float)GetWindowHeight();

    gEditor.controls.bounds.x = EDITOR_PANEL_BORDER;
    gEditor.controls.bounds.y = EDITOR_PANEL_BORDER;
    gEditor.controls.bounds.w = ww - (ww - gEditor.palette.bounds.x) - (EDITOR_PANEL_BORDER*2);
    gEditor.controls.bounds.h = EDITOR_CONTROLS_HEIGHT;

    DrawRect(gEditor.controls.bounds, EDITOR_COLOR1);

    // @Improve: Add more zone types to the game...

    float x = gEditor.controls.bounds.x + EDITOR_CONTROLS_PADDING;
    float y = gEditor.controls.bounds.y + EDITOR_CONTROLS_PADDING;
    SDL_Rect clip = { 0,0,TILE_W,TILE_H };
    DrawImage("editor", x,y, {2,2}, {0,0}, 0.0f, FLIP_NONE, { 1,1,1,1 }, &clip);
}

INTERNAL void DoEditorCanvas ()
{
    float ww = (float)GetWindowWidth();
    float wh = (float)GetWindowHeight();

    // Calculate how large we can make the chunk editor canvas within the available space.

    gEditor.canvas.space.x = EDITOR_PANEL_BORDER;
    gEditor.canvas.space.y = (EDITOR_PANEL_BORDER * 2) + gEditor.controls.bounds.h + 32;
    gEditor.canvas.space.w = ww - (ww - gEditor.palette.bounds.x - EDITOR_PANEL_BORDER);
    gEditor.canvas.space.h = wh - gEditor.canvas.space.y - 32;

    gEditor.canvas.bounds.w = (CHUNK_W * TILE_W);
    gEditor.canvas.bounds.h = (CHUNK_H * TILE_H);

    gEditor.canvas.scale = 1.0f;
    while (gEditor.canvas.bounds.w*(gEditor.canvas.scale+1) < gEditor.canvas.space.w &&
           gEditor.canvas.bounds.h*(gEditor.canvas.scale+1) < gEditor.canvas.space.h)
    {
        gEditor.canvas.scale++;
    }

    gEditor.canvas.bounds.w *= gEditor.canvas.scale;
    gEditor.canvas.bounds.h *= gEditor.canvas.scale;
    gEditor.canvas.bounds.x  = gEditor.canvas.space.x + ((gEditor.canvas.space.w-gEditor.canvas.bounds.w) / 2);
    gEditor.canvas.bounds.y  = gEditor.canvas.space.y + ((gEditor.canvas.space.h-gEditor.canvas.bounds.h) / 2);

    DrawFill(gEditor.canvas.bounds, EDITOR_COLOR0);

    // Handle and draw the tiles and entities in the current chunk.

    float tilew = (TILE_W*gEditor.canvas.scale);
    float tileh = (TILE_H*gEditor.canvas.scale);

    for (int iy=0; iy<CHUNK_H; ++iy)
    {
        for (int ix=0; ix<CHUNK_W; ++ix)
        {
            EditorThing& thing = GetCurrentEditorChunk().things[iy][ix];

            // Lerp values for smooth effects.
            thing.draw.color.current = Lerp(thing.draw.color.current, thing.draw.color.target, gApplication.delta_time*15);
            thing.draw.scale.current = Lerp(thing.draw.scale.current, thing.draw.scale.target, gApplication.delta_time*30);
            thing.draw.angle.current = Lerp(thing.draw.angle.current, thing.draw.angle.target, gApplication.delta_time*15);

            if (thing.draw.scale.current != 0.0f)
            {
                float x = gEditor.canvas.bounds.x + ((float)ix * tilew);
                float y = gEditor.canvas.bounds.y + ((float)iy * tileh);

                float scale = gEditor.canvas.scale * thing.draw.scale.current;

                // Adjust the X and Y for the current scale of the icon so it remains centered.
                x -= ((TILE_W*scale)-(tilew)) / 2;
                y -= ((TILE_H*scale)-(tileh)) / 2;

                Vec2 center = { (TILE_W*scale)/2,(TILE_H*scale)/2 };
                DrawImage(thing.icon_index.type, x,y, {scale,scale}, center, thing.draw.angle.current, FLIP_NONE, thing.draw.color.current, &thing.draw.clip);
            }
        }
    }

    // Handle the mouse cursor drawing and functionality.

    Vec2 mouse = GetMousePos();

    int tilex = (int)floorf((mouse.x-gEditor.canvas.bounds.x) / tilew);
    int tiley = (int)floorf((mouse.y-gEditor.canvas.bounds.y) / tileh);

    gEditor.cursor.pos.x = tilex;
    gEditor.cursor.pos.y = tiley;

    if (PointAndRectCollision(mouse,gEditor.canvas.bounds))
    {
        gEditor.cursor.draw.color.target.a = EDITOR_CURSOR_ALPHA;

        // Handle adding and removing things from the current chunk.
        EditorThing& thing = GetCurrentEditorChunk().things[gEditor.cursor.pos.y][gEditor.cursor.pos.x];
        if (IsMouseButtonDown(SDL_BUTTON_LEFT))
        {
            EditorPlaceThing(thing, gEditor.cursor.selected);
        }
        else if (IsMouseButtonDown(SDL_BUTTON_RIGHT))
        {
            EditorEraseThing(thing);
        }
    }
    else
    {
        gEditor.cursor.draw.color.target.a = 0.0f;
    }

    // Lerp values for smooth effects.
    gEditor.cursor.draw.pos.x = Lerp(gEditor.cursor.draw.pos.x, ((float)tilex*tilew), gApplication.delta_time*20);
    gEditor.cursor.draw.pos.y = Lerp(gEditor.cursor.draw.pos.y, ((float)tiley*tileh), gApplication.delta_time*20);
    gEditor.cursor.draw.color.current.a = Lerp(gEditor.cursor.draw.color.current.a, gEditor.cursor.draw.color.target.a, gApplication.delta_time*40);

    EditorIcon* icon = GetSelectedEditorIcon();
    if (icon)
    {
        float x = gEditor.canvas.bounds.x + gEditor.cursor.draw.pos.x;
        float y = gEditor.canvas.bounds.y + gEditor.cursor.draw.pos.y;

        Vec2 scale = { gEditor.canvas.scale, gEditor.canvas.scale };
        Vec2 center = { 0,0 };

        DrawImage(gEditor.cursor.selected.type, x,y, scale, center, 0.0f, FLIP_NONE, gEditor.cursor.draw.color.current, &icon->draw.clip);
    }
}

INTERNAL void DoEditor ()
{
    // Handle hotkeys.
    if (IsKeyPressed(SDL_SCANCODE_EQUALS)) EditorIncrementZoneChunk();
    if (IsKeyPressed(SDL_SCANCODE_MINUS)) EditorDecrementZoneChunk();

    if (IsKeyDown(SDL_SCANCODE_LCTRL) || IsKeyDown(SDL_SCANCODE_RCTRL))
    {
        if (IsKeyPressed(SDL_SCANCODE_D)) EditorClearZoneChunk();
        if (IsKeyPressed(SDL_SCANCODE_N)) EditorNewZoneChunk();
        if (IsKeyPressed(SDL_SCANCODE_S)) EditorSaveZoneChunks();
    }

    // Update and render editor.
    DoEditorPalette();
    DoEditorControls();
    DoEditorCanvas();
}
