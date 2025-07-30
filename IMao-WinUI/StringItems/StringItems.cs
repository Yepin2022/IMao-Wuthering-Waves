using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace IMao_WinUI.StringItems;

class ItemDatas
{
    public String Id
    {
        get;
    }

    public String Name_SpecifiedLanguage
    {
        get;
    }

    public ItemDatas(String Id, String Name_SpecifiedLanguage)
    {
        this.Id = Id;
        this.Name_SpecifiedLanguage = Name_SpecifiedLanguage;
    }
}

class ItemsDatas
{
    public String Category
    {
        get;
    }

    public List<ItemDatas> ItemDatas
    {
        get;
    }

    public ItemsDatas(String Category, List<ItemDatas> ItemDatas)
    {

        this.Category = Category;
        this.ItemDatas = ItemDatas;
    }
}

class StringItem
{
    private readonly JsonDocument jsonData;

    public List<ItemsDatas> itemsDatas { get; } = new List<ItemsDatas>();
    public StringItem()
    {
        try
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            string resourceName = "IMao_WinUI.StringItems.StringItems.json";

            using Stream stream = assembly.GetManifestResourceStream(resourceName);
            using StreamReader reader = new StreamReader(stream);

            string jsonString = reader.ReadToEnd();

            jsonData = JsonDocument.Parse(jsonString);
        }
        catch(Exception ex)
        {
            Console.WriteLine($"Error loading JSON：{ex.Message}");
        }
    }

    public void LoadString(String specifiedlLanguage)
    {
        try
        {
            foreach (var category in jsonData.RootElement.EnumerateObject())
            {
                List<ItemDatas> itemDatas = new List<ItemDatas>();

                foreach (var item in category.Value.EnumerateObject())
                {
                    foreach (var language in item.Value.EnumerateObject())
                    {
                        var languageCode = language.Name;
                        var translation = language.Value.ToString();

                        if (languageCode == specifiedlLanguage)
                        {
                            itemDatas.Add(new ItemDatas(item.Name, translation));
                            //Console.WriteLine($"    {languageCode}: {translation}");
                        }
                    }
                }
                itemsDatas.Add(new ItemsDatas(category.Name, itemDatas));
            }
        }catch(Exception ex)
        {
            Console.WriteLine($"Error loading String：{ex.Message}");
        }
       
    }

    public List<String> GetItemName()
    {
        List<String> itemNames = new List<String>();
        if (itemsDatas.Count != 0)
        {
            foreach (var itemsDatas in itemsDatas)
            {
                foreach (var itemDatas in itemsDatas.ItemDatas)
                {
                    itemNames.Add(itemDatas.Name_SpecifiedLanguage);
                }
            }
        }
        return itemNames;
    }

    public String GetItemID(String itemName)
    {
        String? id = null;
        if (itemsDatas.Count != 0)
        {
            foreach (var itemsDatas in itemsDatas)
            {
                foreach (var itemDatas in itemsDatas.ItemDatas)
                {
                    if (itemDatas.Name_SpecifiedLanguage == itemName)
                    {
                        id = itemDatas.Id;
                        return id;
                    }
                }
            }
        }
        return id;
    }
}
