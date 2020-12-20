GLOBAL std::map<std::string,ItemBase> gItems;

INTERNAL void InitItems ()
{
    gItems.clear();

    GonObject& gon = *GetAsset<AssetData>("items");
    if (gon.type != GonObject::FieldType::OBJECT)
    {
        LOG_ERROR(ERR_MAX, "Item data not formatted as object!");
        return;
    }

    // Go through each base item type and cache the data.
    for (auto& data: gon.children_array)
    {
        // Warn of items with duplicate names.
        if (gItems.count(data.name))
        {
            LOG_ERROR(ERR_MIN, "Warning duplicate item with the name %s!", data.name.c_str());
        }

        ItemBase base = {};

        if (data.Contains("image"))
        {
            int imagex = data["image"][0].Int(0);
            int imagey = data["image"][1].Int(0);

            base.clip.x = imagex*TILE_W;
            base.clip.y = imagey*TILE_H;
            base.clip.w = imagex;
            base.clip.h = imagey;
        }

        if (data.Contains("color"))
        {
            base.color.r = (float)data["color"][0].Number(255.0) / 255.0f;
            base.color.g = (float)data["color"][1].Number(255.0) / 255.0f;
            base.color.b = (float)data["color"][2].Number(255.0) / 255.0f;
            base.color.a = (float)data["color"][3].Number(255.0) / 255.0f;
        }
        else
        {
            base.color = { 1,1,1,1 };
        }

        gItems.insert({ data.name, base });
    }
}
