using IMao_WinUI.Helpers;
using Microsoft.UI.Xaml;
using Windows.UI.ViewManagement;

namespace IMao_WinUI;

public sealed partial class MainWindow : WindowEx
{
    private Microsoft.UI.Dispatching.DispatcherQueue dispatcherQueue;

    private bool _isFirstActivation = true;
    private UISettings settings;

    public MainWindow()
    {
        InitializeComponent();
        this.Activated += MainWindow_Activated;
        AppWindow.SetIcon(Path.Combine(AppContext.BaseDirectory, "Assets/WindowIcon.ico"));
        Content = null;
        Title = "AppDisplayName".GetLocalized();

        // Theme change code picked from https://github.com/microsoft/WinUI-Gallery/pull/1239
        dispatcherQueue = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread();
        settings = new UISettings();
        settings.ColorValuesChanged += Settings_ColorValuesChanged; // cannot use FrameworkElement.ActualThemeChanged event
        //Dll初始化
        IMaoCoreAPI.Initi();
    }

    private void MainWindow_Activated(object sender, WindowActivatedEventArgs args)
    {
        if (_isFirstActivation && args.WindowActivationState == WindowActivationState.CodeActivated)
        {
            this.CenterOnScreen();
            _isFirstActivation = false;
        }
    }

    // this handles updating the caption button colors correctly when indows system theme is changed
    // while the app is open
    private void Settings_ColorValuesChanged(UISettings sender, object args)
    {
        // This calls comes off-thread, hence we will need to dispatch it to current app's thread
        dispatcherQueue.TryEnqueue(() =>
        {
            TitleBarHelper.ApplySystemThemeToCaptionButtons();
        });
    }
}
