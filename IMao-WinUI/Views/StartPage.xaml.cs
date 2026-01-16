using IMao_WinUI.Contracts.Services;
using IMao_WinUI.Helpers;
using IMao_WinUI.StringItems;
using IMao_WinUI.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Diagnostics;
using System.Drawing;
namespace IMao_WinUI.Views;

public sealed partial class StartPage : Page
{
    private bool isRunning = true;
    public StartViewModel ViewModel
    {
        get;
    }

    public StartPage()
    {
        ViewModel = App.GetService<StartViewModel>();
        InitializeComponent();
        ComboBox_GameServer.SelectedIndex = 0;
        ComboBox_CaptureMethod.SelectedIndex = 0;
        _ = InitializeAsync();

        LocalItemFilter localItemFilter = new LocalItemFilter();
        var filteredItemsDatas = localItemFilter.GetFilteredItemsDatas();

        IMaoCoreAPI.EnabledMinMapShowItem(true);
        IMaoCoreAPI.EnabledMapShowItem(true);
        IMaoCoreAPI.SetMapDataUpdateCycle(80);
        IMaoCoreAPI.SetMinMapDataUpdateCycle(80);

        foreach (var filteredItemDatas in filteredItemsDatas)
        {
            if (filteredItemDatas.Status == 1)
            {
                IMaoCoreAPI.AddItem(filteredItemDatas.Name);
            }
        }
    }

    private async Task InitializeAsync()
    {
        var isLatest = await CheckVersion.IsLatest();
        if (!isLatest)
        {
            Start_InfoBar_NotLatestVersion.IsOpen = true;
        }
    }
    private void ComboBox_CaptureMethod_Loaded(object sender, RoutedEventArgs e)
    {
       
    }

    private void ComboBox_GameServer_Loaded(object sender, RoutedEventArgs e)
    {
        
    }

    private async void Start_Button_Click(object sender, RoutedEventArgs e)
    {
        Button button = (Button)sender;
        StackPanel contentPanel = (StackPanel)button.Content;
        FontIcon icon = (FontIcon)contentPanel.Children[0];
        TextBlock textBlock = (TextBlock)contentPanel.Children[1];

        if (isRunning)
        {
            ContentDialog dialog = new ContentDialog();

            // XamlRoot must be set in the case of a ContentDialog running in a Desktop app
            dialog.XamlRoot = this.XamlRoot;
            dialog.Style = Application.Current.Resources["DefaultContentDialogStyle"] as Style;
            dialog.Title = "Suggestion";
            dialog.PrimaryButtonText = "Confirm";
            dialog.SecondaryButtonText = "I see";
            dialog.DefaultButton = ContentDialogButton.Primary;
            dialog.Content = new SuggestionDialog();
            var result = await dialog.ShowAsync();

            if (IMaoCoreAPI.Start()==0)
            {
                if (Start_ToggleSwitch_WindowOptimization.IsOn)
                {
                    BitBltRegistryHelper.SetDirectXUserGlobalSettings(); 
                }
                Start_InfoBar_FindNotTargetProcess.IsOpen = true;
                Start_InfoBar_FindNotTargetProcess.Margin = new Thickness(0, 0, 0, 12);
                return;
            }
            isRunning = !isRunning;
            icon.Glyph = "\uE71A";
            textBlock.Text = "Stop";
            IMaoCoreAPI.SetCaptureWay(ComboBox_CaptureMethod.SelectedIndex);

            if (!GameWindow.CheckGameWindowSize())
            {
                Start_InfoBar_IncorrectGameWindowSize.IsOpen = true;
                Start_InfoBar_IncorrectGameWindowSize.Margin = new Thickness(0, 0, 0, 12);
            }
        }
        else
        {
            isRunning = !isRunning;
            icon.Glyph = "\uE768"; 
            textBlock.Text = "Start";
            IMaoCoreAPI.Stop();
        }
    }


    private void Start_InfoBar_FindNotTargetProcess_CloseButtonClick(InfoBar sender, object args)
    {
        Start_InfoBar_FindNotTargetProcess.Margin = new Thickness(0, 0, 0, 0);
    }

    private void Start_InfoBar_IncorrectGameWindowSize_CloseButtonClick(InfoBar sender, object args)
    {
        Start_InfoBar_IncorrectGameWindowSize.Margin = new Thickness(0, 0, 0, 0);
    }

    private void Button_OpenPointsFolder_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            string appDirectory = AppDomain.CurrentDomain.BaseDirectory;

            string routesPath = Path.Combine(appDirectory, "SavedPoints");

            if (Directory.Exists(routesPath))
            {
                Process.Start(new ProcessStartInfo(routesPath)
                {
                    UseShellExecute = true,
                    Verb = "open"
                });
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Button_OpenPointsFolder_Click:" + ex.Message);
        }
    }
}
