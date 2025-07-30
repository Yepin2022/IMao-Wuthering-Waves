using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

public static class IMaoCoreAPI
{
    private const string DllName = "IMao-Core.dll";
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Initi();
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Start();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetMapDataUpdateCycle(int cycleTime);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetMinMapDataUpdateCycle(int cycleTime);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetCaptureWay(int setvalue);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Stop();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void EnabledMinMapShowItem(bool setValue);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void EnabledMapShowItem(bool setValue);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void AddItem(String itemId);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void ClearItem(String itemId);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetVisibleSavedPoints(bool setValue);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    private static extern void GetMyMessage([Out] StringBuilder buffer, int bufferSize);

    public static string GetDllMessage()
    {
        StringBuilder buffer = new StringBuilder(256);
        GetMyMessage(buffer, buffer.Capacity);
        return buffer.ToString();
    }
}
