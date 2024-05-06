// External libs
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "wx/app.h"
#include <thread>

// internal files
#include "Settings.h"

// DEFAULTS
#define DEFAULT_PORT "27015"
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_BUFLEN 512

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};


class ChatWindow : public wxFrame
{
public:
    bool is_logged_in;

    ChatWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size), mainPanel(nullptr), client(new Client), is_logged_in(false)
    {
        // main panel
        mainPanel = new wxPanel(this, wxID_ANY);

        // login section
        loginPanel = new wxPanel(mainPanel, wxID_ANY);
        wxBoxSizer* loginSizer = new wxBoxSizer(wxVERTICAL);
        loginPanel->SetSizer(loginSizer);

        wxStaticText* loginLabel = new wxStaticText(loginPanel, wxID_ANY, wxT("Login:"));
        loginSizer->Add(loginLabel, 0, wxALL, 5);
        loginInput = new wxTextCtrl(loginPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        loginSizer->Add(loginInput, 0, wxEXPAND | wxALL, 5);

        wxStaticText* passwordLabel = new wxStaticText(loginPanel, wxID_ANY, wxT("Password:"));
        loginSizer->Add(passwordLabel, 0, wxALL, 5);
        passwordInput = new wxTextCtrl(loginPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
        loginSizer->Add(passwordInput, 0, wxEXPAND | wxALL, 5);

        loginButton = new wxButton(loginPanel, wxID_ANY, wxT("Login"));
        loginSizer->Add(loginButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        // active chat rooms section
        roomListPanel = new wxPanel(mainPanel, wxID_ANY);
        wxBoxSizer* roomListSizer = new wxBoxSizer(wxVERTICAL);
        roomListPanel->SetSizer(roomListSizer);

        wxStaticText* roomListLabel = new wxStaticText(roomListPanel, wxID_ANY, wxT("Available Chat Rooms:"));
        roomListSizer->Add(roomListLabel, 0, wxALL, 5);
        roomListView = new wxListView(roomListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        roomListSizer->Add(roomListView, 1, wxEXPAND | wxALL, 5);

        // set main sizer
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        mainSizer->Add(loginPanel, 1, wxEXPAND | wxALL, 5);
        mainSizer->Add(roomListPanel, 1, wxEXPAND | wxALL, 5);
        mainPanel->SetSizer(mainSizer);

        // bind login event
        loginButton->Bind(wxEVT_BUTTON, &ChatWindow::OnLogin, this);
    }

    void OnLogin(wxCommandEvent& event)
    {
        wxString login = loginInput->GetValue();
        wxString password = passwordInput->GetValue();

        // Checking credentials logic here
        client->connectToServer(DEFAULT_ADDR, DEFAULT_PORT, (std::string)login, (std::string)password);


        // empty check
        if (login.IsEmpty() || password.IsEmpty())
        {
            wxMessageBox("Please enter both login and password.", "Login Error", wxOK | wxICON_ERROR);
            return;
        }

        // handle server log in
        client->logIn((std::string) login, (std::string) password);
        is_logged_in = true;
        listenToServer();

        // removing panels
        mainPanel->GetSizer()->Detach(loginPanel); 
        mainPanel->GetSizer()->Detach(roomListPanel);
        loginPanel->Destroy();
        roomListPanel->Destroy();

        // room entering panel
        roomPanel = new wxPanel(mainPanel, wxID_ANY);
        wxBoxSizer* roomSizer = new wxBoxSizer(wxVERTICAL);
        roomPanel->SetSizer(roomSizer);

        // Adding room id input
        wxStaticText* roomLabel = new wxStaticText(roomPanel, wxID_ANY, wxT("Room id:"));
        roomSizer->Add(roomLabel, 0, wxALL, 5);
        roomInput = new wxTextCtrl(roomPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        roomSizer->Add(roomInput, 0, wxEXPAND | wxALL, 5);

        joinButton = new wxButton(roomPanel, wxID_ANY, wxT("join"));
        roomSizer->Add(joinButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
        joinButton->Bind(wxEVT_BUTTON, &ChatWindow::OnJoin, this);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        mainSizer->Add(roomPanel, 1, wxEXPAND | wxALL, 5);
        mainPanel->SetSizer(mainSizer);

        mainPanel->Layout();
    }

    void OnJoin(wxCommandEvent& event) {
        wxString room_id = roomInput->GetValue();

        if (room_id.IsEmpty())
        {
            wxMessageBox("Please enter room id.", "Room Entering Error", wxOK | wxICON_ERROR);
            return;
        }

        // handle server log in
        client->joinRoom((std::string)room_id);

        // remove panels
        mainPanel->GetSizer()->Detach(roomPanel); 
        roomPanel->Destroy(); 

        // create chat panel
        wxPanel* chatPanel = new wxPanel(mainPanel, wxID_ANY);
        wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);
        chatPanel->SetSizer(chatSizer);

        chatDisplay = new wxTextCtrl(chatPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
        chatSizer->Add(chatDisplay, 1, wxEXPAND | wxALL, 5);

        messageInput = new wxTextCtrl(chatPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        chatSizer->Add(messageInput, 0, wxEXPAND | wxALL, 5);

        // message sending
        messageInput->Bind(wxEVT_TEXT_ENTER, &ChatWindow::SendMessage, this);

        //messageInput->Bind(wxEVT_TEXT, &ChatWindow::OnTextChanged, this);
        //suggestionsList = new wxListBox(chatPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE);
        //suggestionsList->Hide();
        //chatSizer->Add(suggestionsList, 0, wxEXPAND | wxALL, 5);


        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        mainSizer->Add(chatPanel, 1, wxEXPAND | wxALL, 5);
        mainPanel->SetSizer(mainSizer);

        mainPanel->Layout();
    }

    void SendMessage(wxCommandEvent& event)
    {
        // getting message
        messageInput = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
        wxString message = messageInput->GetValue();

        // Sending message to server
        client->handleInput((std::string) message);

        // Append chat to display
        chatDisplay->AppendText("You: " + message + "\n");

        // clear input on send
        messageInput->Clear();
    }

    void ReceiveMessage(const wxString& message, const wxString& username)
    {
        // append received message from server
        chatDisplay->AppendText(username + ": " + message + "\n");
    }

    // Server listener on differend detached thread
    void listenToServer() {
        if (is_logged_in) {

            // listener
            const auto f = [this]() {
                while (1) {
                    std::vector<std::string> data = client->getServerChatMessage();

                    if (data.size() == 2) {
                        CallAfter([this, data]() {
                            this->ReceiveMessage(data[1], data[0]);
                            });
                    }
                }
            };

            // thread with listener
            std::thread bck{ f };
            bck.detach();
        }
    }


    // TODO: show suggestions
    void OnTextChanged(wxCommandEvent& event)
    {
        wxString text = messageInput->GetValue();
        if (!text.IsEmpty() && text.StartsWith("/"))
        {
            suggestionsList->Show();
            suggestionsList->Clear();
            suggestionsList->Append("1");
            suggestionsList->Append("2");
            suggestionsList->Append("3");
        }
        else
        {
            suggestionsList->Hide();
        }

        Layout();
    }

private:

    // UI variables
    
    // main panel
    wxPanel* mainPanel;
    
    // login page
    wxPanel* loginPanel;
    wxTextCtrl* loginInput;
    wxTextCtrl* passwordInput;
    wxButton* loginButton;
    
    // chat room preview
    wxPanel* roomListPanel;
    wxListView* roomListView;

    // chat connecting page
    wxPanel* roomPanel;
    wxTextCtrl* roomInput;
    wxButton* joinButton;

    // chat room
    wxTextCtrl* chatDisplay;
    wxListBox* suggestionsList;

    // client 
    Client* client;
    wxTextCtrl* messageInput;
};


wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    Client client;
    ChatWindow* chatWindow = new ChatWindow(wxT("Chat Application - Client"), wxPoint(50, 50), wxSize(800, 600));
    chatWindow->Show(true);

    return true;
}

