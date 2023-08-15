#include "Tracker.h"
#include "SoundGenerator.h"

//Universal variables here
SoundGenerator SG(1, 56, 1);
bool running = true;

//Screen dimension constants
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_AudioSpec want, have;
SDL_AudioDeviceID dev;

Tracker::Tracker()
{
}

Tracker::~Tracker()
{

}

void Tracker::Initialise(int StartLength)
{
	//initialise all the channels
	for (size_t i = 0; i < 8; i++)
	{
		Channel channel = Channel();
		channel.SetUp(StartLength);
		Channels[i] = channel;
	}
}

void Tracker::Run(void)
{
	DefaultInst.Index = 0;
	DefaultInst.Name = "Instrument: ";
	DefaultInst.SampleIndex = 0;
	DefaultInst.Volume = 127;
	DefaultInst.LPan = 127;
	DefaultInst.RPan = 127;
	DefaultInst.NoiseFreq = 0;
	DefaultInst.Gain = 0;
	DefaultInst.InvL = false;
	DefaultInst.InvR = false;
	DefaultInst.PitchMod = false;
	DefaultInst.Echo = false;
	DefaultInst.Noise = false;
	inst.push_back(DefaultInst);

	bool PlayingTrack = false;
	bool WindowIsGood = true;

	//ImGUI setup
	IMGUI_CHECKVERSION();
	cont = ImGui::CreateContext();
	SetCurrentContext(cont);
	StyleColorsDark();
	ImGuiIO IO = ImGui::GetIO();	
	io = IO;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.DisplaySize.x = SCREEN_WIDTH;
	io.DisplaySize.y = SCREEN_HEIGHT;
	io.DeltaTime = 1.f / 60.f;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		WindowIsGood = false;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("CrispyTracker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
		rend = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC);
		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(window, rend);
		ImGui_ImplSDLRenderer2_Init(rend);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			WindowIsGood = false;
		}
		else
		{
			//Load fonts
			ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/Inconsolata.ttf", TextSize, NULL, NULL);
			io.Fonts->Build();
			ImGui_ImplSDLRenderer2_CreateFontsTexture();
			WindowIsGood = true;
		}
	}

	//Initialise the tracker
	Initialise(8);
	while (running) {
		if (WindowIsGood) {
			Render();
		}
		CheckInput();
	}

	//Destroy window
	SDL_DestroyWindow(window);
	DestroyContext();
	//Quit SDL subsystems
	SDL_Quit();
}

void Tracker::CheckInput()
{
	int TuninOff = 48;
	SDL_Event event;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			default:
				return;
				break;
			case SDL_QUIT:
				running = false;
				break;
				//upper octave
			case SDLK_q:
				SG.NoteIndex = TuninOff;
				printf("PRESSED Q");
				SG.PlayingNoise = true;
				break;
			case SDLK_2:
				SG.NoteIndex = TuninOff + 1;
				printf("PRESSED 1");
				SG.PlayingNoise = true;
				break;
			case SDLK_w:
				SG.NoteIndex = TuninOff + 2;
				printf("PRESSED W");
				SG.PlayingNoise = true;
				break;
			case SDLK_3:
				SG.NoteIndex = TuninOff + 3;
				printf("PRESSED 3");
				SG.PlayingNoise = true;
				break;
			case SDLK_e:
				SG.NoteIndex = TuninOff + 4;
				printf("PRESSED E");
				SG.PlayingNoise = true;
				break;
			case SDLK_r:
				SG.NoteIndex = TuninOff + 5;
				printf("PRESSED R");
				SG.PlayingNoise = true;
				break;
			case SDLK_5:
				SG.NoteIndex = TuninOff + 6;
				printf("PRESSED 5");
				SG.PlayingNoise = true;
				break;
			case SDLK_t:
				SG.NoteIndex = TuninOff + 7;
				printf("PRESSED T");
				SG.PlayingNoise = true;
				break;
			case SDLK_6:
				SG.NoteIndex = TuninOff + 8;
				printf("PRESSED 6");
				SG.PlayingNoise = true;
				break;
			case SDLK_y:
				SG.NoteIndex = TuninOff + 9;
				printf("PRESSED Y");
				SG.PlayingNoise = true;
				break;
			case SDLK_7:
				SG.NoteIndex = TuninOff + 10;
				printf("PRESSED 7");
				SG.PlayingNoise = true;
				break;
			case SDLK_u:
				SG.NoteIndex = TuninOff + 11;
				printf("PRESSED U");
				SG.PlayingNoise = true;
				break;
			case SDLK_i:
				SG.NoteIndex = TuninOff + 12;
				printf("PRESSED I");
				SG.PlayingNoise = true;
				break;
			}
			SG.CheckSound(want, have, dev, Channels);
			SDL_PauseAudioDevice(dev, 0);
			SDL_Delay(1);

			break;
		case SDL_KEYUP:
			SG.PlayingNoise = false;
			SDL_PauseAudioDevice(dev, 1);
			break;

		case SDL_QUIT:
			running = false;
			break;
		}
	}
}

void Tracker::Render()
{
	ImGui_ImplSDL2_NewFrame();
	NewFrame();

	MenuBar();
	Patterns();
	Instruments();
	Instrument_View();
	Samples();
	Sample_View();
	Settings_View();

	SDL_RenderSetScale(rend, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(rend,22, 22, 22, 255);
	ImGui::Render();
	SDL_RenderClear(rend);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(rend);
	EndFrame();
}

void Tracker::MenuBar()
{
	BeginMainMenuBar();
	if (BeginMenu("File"))
	{
		ImGui::MenuItem("Load");
		ImGui::MenuItem("Save");
		ImGui::MenuItem("Save As");
		ImGui::MenuItem("Export Wav");
		ImGui::MenuItem("Export SPC");
		ImGui::EndMenu();
	}

	if (BeginMenu("Edit"))
	{
		ImGui::EndMenu();
	}

	if (BeginMenu("Settings"))
	{
		Text("Settings :3");
		ImGui::EndMenu();
	}

	if (BeginMenu("Help"))
	{
		ImGui::MenuItem("Effects List");
		ImGui::MenuItem("Manual");
		ImGui::MenuItem("Credits");
		ImGui::EndMenu();
	}
	EndMainMenuBar();

}

void Tracker::Patterns()
{
	if (Begin("Patterns"), true, UNIVERSAL_WINDOW_FLAGS)
	{
		Columns(8);
		for (char i = 0; i < 8; i++)
		{
			Text(to_string(i).data());
			NextColumn();
		}
		End();
	}
	else
	{
		End();
	}
}

void Tracker::Instruments()
{
	if (Begin("Instruments"), true, UNIVERSAL_WINDOW_FLAGS)
	{
		if (Button("Add", ImVec2(GetWindowWidth()*0.33, 24)))
		{
			Instrument newinst = DefaultInst;
			int index = inst.size();
			newinst.Name += to_string(index);
			inst.push_back(newinst);
			cout << inst.size();
		}
		SameLine();
		if (Button("Delete", ImVec2(GetWindowWidth() * 0.33, 24)) && inst.size() > 1)
		{
			inst.pop_back();

			if (SelectedInst > inst.size())
			{
				SelectedInst = inst.size() - 1;
			}
		}		
		SameLine();
		if (Button("Copy", ImVec2(GetWindowWidth() * 0.33, 24)) && inst.size() > 1)
		{
			int index = inst.size();
			Instrument newinst = DefaultInst;
			newinst.Name += to_string(index);
			inst.push_back(newinst);
			cout << inst.size();
		}

		if (inst.size() > 0)
		{
			BeginChild("List", ImVec2(GetWindowWidth() - InstXPadding, GetWindowHeight() - InstYPadding), true, UNIVERSAL_WINDOW_FLAGS);
			BeginTable("InstList", 1, TABLE_FLAGS, ImVec2(128, 24), 24);
			for (char i = 0; i < inst.size(); i++)
			{
				Text(to_string(i).data());
				SameLine();
				if (Button(inst[i].Name.data()))
				{
					SelectedInst = i;
					ShowInstrument = true;
				}
				TableNextColumn();
			}
			EndTable();
			EndChild();
		}
		End();
	}
	else
	{
		End();
	}
}

void Tracker::Instrument_View()
{
	if (ShowInstrument)
	{
		if (Begin("Instrument Editor"), true, UNIVERSAL_WINDOW_FLAGS)
		{
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * .75);
			InputText("InstName", (char*)inst[SelectedInst].Name.data(), sizeof(inst[SelectedInst].Name.data())*8);

			SliderInt("Volume", &inst[SelectedInst].Volume, 0, 127);
			SliderInt("Gain", &inst[SelectedInst].Gain, 0, 255);

			Checkbox("Envelope used", &inst[SelectedInst].EnvelopeUsed);
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.33);

			if (inst[SelectedInst].EnvelopeUsed)
			{
				NewLine();
				Text("Envelope");
				SliderInt("Attack ", &inst[SelectedInst].Attack, 0, 15);
				SameLine();
				SliderInt("Decay", &inst[SelectedInst].Decay, 0, 7);
				
				SliderInt("Sustain", &inst[SelectedInst].Sustain, 0, 31);
				SameLine();
				SliderInt("Release", &inst[SelectedInst].Release, 0, 31);


				SliderInt("Left   ", &inst[SelectedInst].LPan, 0, 127);
				SameLine();
				SliderInt("Right", &inst[SelectedInst].RPan, 0, 127);
			}

			NewLine();
			Text("Special");
			NewLine();
			Checkbox("Invert L  ", &inst[SelectedInst].InvL);
			SameLine();
			Checkbox("Invert R", &inst[SelectedInst].InvR);

			Checkbox("Pitch Mod ", &inst[SelectedInst].PitchMod);
			SameLine();
			Checkbox("Echo", &inst[SelectedInst].Echo);

			Checkbox("Noise     ", &inst[SelectedInst].Noise);
			if (inst[SelectedInst].Noise)
			{
				SameLine();
				SliderInt("Noise Freq ", &inst[SelectedInst].NoiseFreq, 0, 31);
			}			
			End();
		}
		else
		{
			ShowInstrument = false;
			End();
		}
	}

}

void Tracker::Samples()
{
}

void Tracker::Sample_View()
{
}

void Tracker::Settings_View()
{
	if (ShowSettings)
	{

	}
	else
	{

	}
}

void Tracker::Credits()
{
}


/*
if (Begin("Main"), true, UNIVERSAL_WINDOW_FLAGS)
{
	Text("This is some text");
	BeginChild("Sub", ImVec2(240, 120), true, UNIVERSAL_WINDOW_FLAGS);
		Text("More text");
		EndChild();
	End();
}
else
{
	End();
}
*/

/*
//ImGui::ShowDemoWindow();
{
	Begin("Main Window", &ShowMain);

	Text("This is some useful text.");               // Display some text (you can use a format strings too)
	Checkbox("Demo Window", &ShowMain);      // Edit bools storing our window open/close state
	Checkbox("Another Window", &ShowMain);

	End();
}
*/