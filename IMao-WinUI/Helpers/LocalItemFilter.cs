using IMao_WinUI.Core.Helpers;
using ServiceStack;
using ServiceStack.Script;
using System;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http.Json;
using System.Reflection;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace IMao_WinUI.Helpers;

class FilterItemDatas
{
    public String? Name { get; set; }
    public int Status { get; set; }

    public FilterItemDatas(String? name, int status)
    {
        Name = name;
        Status = status;
    }
}

class LocalItemFilter
{
    private String filterJsonFileName = new String("FilteredItemsData.json");
    private JsonDocument? filterJsonData;
    private bool isParseSuccess = false;
    private String filterJsonFath;

    public LocalItemFilter()
    {
        isParseSuccess = ParseFilterJsonFile();
    }

    private String GetRunningDirectory()
    {
        try
        {
            String? executablePath = Assembly.GetEntryAssembly()?.Location;

            if(executablePath == null)
            {
                executablePath = Assembly.GetExecutingAssembly().Location;
            }

            String? directory = Path.GetDirectoryName(executablePath);

            return directory ?? String.Empty;
        }
        catch (Exception ex)
        {
            return String.Empty;
        }
    }

    private bool ParseFilterJsonFile()
    {
        String? runningDirectory = GetRunningDirectory();

        if (runningDirectory == null)
        {
            return false;
        }

        try
        {
            filterJsonFath = runningDirectory + "\\" + filterJsonFileName;

            if (!File.Exists(filterJsonFath)){ return false; }

            String? filterJsonString = File.ReadAllText(filterJsonFath);

            if (filterJsonString == null)
            {
                return false;
            }
            
            filterJsonData = JsonDocument.Parse(filterJsonString);
            return true;

        }catch(Exception){ return false; }
    }

    public List<FilterItemDatas> GetFilteredItemsDatas()
    {
        List<FilterItemDatas> filterItemsDatas = new List<FilterItemDatas>();
        try
        {
            if (!isParseSuccess) { return filterItemsDatas; }

            foreach (var category in filterJsonData.RootElement.EnumerateObject())//Status
            {
                foreach (var filteredItem in category.Value.EnumerateObject())
                {
                    String filteredItemName = filteredItem.Name;
                    String filteredItemValue = filteredItem.Value.ToString();
                    filterItemsDatas.Add(new FilterItemDatas(filteredItemName, filteredItemValue.ToInt()));
                }
            }
            return filterItemsDatas;

        }
        catch (Exception) {

            return filterItemsDatas; 
        }
    }


    public void SetItmeFilterStatus(String itemName,int statusValue)
    {
        try
        {
            String updateJsonString;

            if (isParseSuccess)
            {
                JsonObject? rootObj = JsonNode.Parse(filterJsonData.RootElement.GetRawText())?.AsObject();

                JsonObject? statusObj = rootObj["Status"]?.AsObject();

                if (statusObj == null)
                {
                    rootObj["Status"] = new JsonObject();

                    statusObj = rootObj["Status"]!.AsObject();
                }

                statusObj[itemName] = statusValue;

                updateJsonString = rootObj.ToJsonString();
                filterJsonData = JsonDocument.Parse(updateJsonString);
            }
            else
            {
                JsonObject newRoot = new JsonObject
                {
                    ["Status"] = new JsonObject()
                };

                JsonObject statusObj = newRoot["Status"]!.AsObject();
                statusObj[itemName] = statusValue;

                updateJsonString = newRoot.ToJsonString();
                filterJsonData = JsonDocument.Parse(updateJsonString);
                isParseSuccess = true;
            }
           
            File.WriteAllText(filterJsonFath, updateJsonString);
        }
        catch (Exception) { return; }
    }
}