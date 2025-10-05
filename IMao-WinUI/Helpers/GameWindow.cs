using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Drawing;

class WindowClientSizeGetter
{
    // 引入Windows API
    [DllImport("user32.dll")]
    private static extern bool EnumWindows(EnumWindowsProc enumProc, IntPtr lParam);

    [DllImport("user32.dll")]
    private static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    [DllImport("user32.dll")]
    private static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

    private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")]
    private static extern bool IsWindowVisible(IntPtr hWnd);

    // 客户区矩形结构
    private struct RECT
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    /// <summary>
    /// 通过进程名获取窗口客户区大小（不含非客户区域）
    /// </summary>
    /// <param name="processName">进程名（不含.exe）</param>
    /// <returns>客户区大小，未找到返回Size.Empty</returns>
    public static Size GetClientSizeByProcessName(string processName)
    {
        try
        {
            var process = Process.GetProcessesByName(processName).FirstOrDefault();
            if (process == null)
            {
                return Size.Empty;
            }
            uint targetProcessId = (uint)process.Id;

            IntPtr targetHwnd = IntPtr.Zero;

            EnumWindows((hWnd, lParam) =>
            {
                GetWindowThreadProcessId(hWnd, out uint processId);

                if (processId == targetProcessId && IsWindowVisible(hWnd))
                {
                    targetHwnd = hWnd;
                    return false; 
                }
                return true;
            }, IntPtr.Zero);

            if (targetHwnd == IntPtr.Zero)
            {
                return Size.Empty;
            }

            // 获取客户区大小
            if (GetClientRect(targetHwnd, out RECT clientRect))
            {
                int width = clientRect.Right - clientRect.Left;
                int height = clientRect.Bottom - clientRect.Top;
                return new Size(width, height);
            }

            return Size.Empty;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"GetClientSizeByProcessName: {ex.Message}");
            return Size.Empty;
        }
    }

}

class GameWindow()
{
    public static bool CheckGameWindowSize()
    {
        Size clientSize = WindowClientSizeGetter.GetClientSizeByProcessName("Client-Win64-Shipping");

        if (clientSize != Size.Empty)
        {
            float a = (float)clientSize.Width / (float)clientSize.Height;
            if (1.76 < a && a < 1.78)
            {
                return true;
            }
        }

        return false;
    }
}