using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;

namespace IMao_WinUI.Helpers;
public class BitBltRegistryHelper
{
    public static void SetDirectXUserGlobalSettings()
    {
        try
        {
            const string keyPath = @"Software\Microsoft\DirectX\UserGpuPreferences";
            const string valueName = "DirectXUserGlobalSettings";
            const string valueData = "SwapEffectUpgradeEnable=0;";

            using var key = Registry.CurrentUser.CreateSubKey(keyPath);
            key.SetValue(valueName, valueData, RegistryValueKind.String);
        }
        catch (Exception e)
        {
            Debug.WriteLine(e);
        }
    }
}

