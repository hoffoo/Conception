template <typename T> ListWidget<T>::ListWidget(Vector2n Position, std::vector<T> & List, TypingModule & TypingModule)
	: CompositeWidget(Position, {
		/*std::shared_ptr<Widget>(new ButtonWidget(Vector2n(-1, -18), Vector2n(lineHeight, lineHeight), [&]() {
			// TEST: This is specific stuff for quick testing
			if (!m_List.empty())
			{
				m_List.pop_back();
			}
		} ))*/ }),
	  m_TapAction(),
	  m_List(List),
	  m_TypingModule(TypingModule)
{
	ModifyGestureRecognizer().m_RecognizeTap = true;
	ModifyGestureRecognizer().m_RecognizeManipulationTranslate = true;

	UpdateDimensions();
}

template <typename T> ListWidget<T>::~ListWidget()
{
}

template <typename T> void ListWidget<T>::Insert(T & Entry)
{
	m_List.push_back(Entry);
}

template <typename T> void ListWidget<T>::Render()
{
	Color BackgroundColor(1, 1, 1);
	Color BorderColor(0.3, 0.3, 0.3);

	/*if (CheckHover(WidgetManager) && CheckActive(WidgetManager))
	{
	}
	else if ((CheckHover(WidgetManager) && !CheckAnyActive(WidgetManager)) || (!CheckHover(WidgetManager) && CheckActive(WidgetManager)))
	{
		BorderColor[0] = 0.898;
		BorderColor[1] = 0.765;
		BorderColor[2] = 0.396;
	}
	else
	{
	}*/

	// TODO: Think if I should outsource the entire rendering code for more generality
	// Draw list
	{
		UpdateDimensions();		// LATER: Optimize by not repeating this calculation each time, only when something changes?

		if (m_List.empty())
		{
			BackgroundColor[0] = 234 / 255.0;
			BackgroundColor[1] = 233 / 255.0;
			BackgroundColor[2] = 190 / 255.0;
		}

		DrawAroundBox(GetPosition(), GetDimensions(), BackgroundColor, BorderColor);

		// TODO: This part is not general
		std::string Description[2] = { "#include <", ">" };
		glColor3d(0, 0, 0); OglUtilsPrint(GetPosition().X(), GetPosition().Y(), 0, RIGHT, Description[0].c_str());
		glColor3d(0, 0, 0); OglUtilsPrint(GetPosition().X() + GetDimensions().X(), GetPosition().Y(), 0, LEFT, Description[1].c_str());
		
		// TEST
		auto Spot = m_List.end();
		if (!m_TypingModule.GetString().empty())
		{
			for (auto & Pointer : GetGestureRecognizer().GetConnected())
			{
				if (Pointer::VirtualCategory::POINTING == Pointer->GetVirtualCategory())
				{
					Vector2n GlobalPosition(Pointer->GetPointerState().GetAxisState(0).GetPosition(), Pointer->GetPointerState().GetAxisState(1).GetPosition());
					Vector2n LocalPosition(GlobalToLocal(GlobalPosition));

					Spot = m_List.begin() + (LocalPosition.Y() / lineHeight);
				}
			}
		}

		OpenGLStream OpenGLStream(GetPosition());
		//for (auto & Entry : m_List)
		for (auto Entry = m_List.begin(); m_List.end() != Entry; ++Entry)
		{
			if (Entry == Spot)
				OpenGLStream << endl;

			OpenGLStream << *Entry << endl;
		}
	}

	CompositeWidget::Render();
}

template <typename T> void ListWidget<T>::ProcessTap(InputEvent & InputEvent, Vector2n Position)
{
	/*if (nullptr != m_TapAction)
	{
		m_TapAction();
	}*/

	auto LocalPosition = GlobalToLocal(Position);

	auto Entry = m_TypingModule.GetString();
	m_TypingModule.Clear();

	if (!Entry.empty())
	{
		auto ConceptId = FindOrCreateConcept(Entry);

		//Insert(ConceptId);
		
		// TEST
		auto Spot = m_List.begin() + (LocalPosition.Y() / lineHeight);
		m_List.insert(Spot, ConceptId);
	}
	else
	{
		auto ListEntry = static_cast<decltype(m_List.size())>(LocalPosition.Y() / lineHeight);

		if (ListEntry < m_List.size())
		{
			m_TypingModule.SetString(Concepts[m_List[ListEntry]].m_Concept);
			m_List.erase(m_List.begin() + ListEntry);
		}
	}
}

template <typename T> void ListWidget<T>::UpdateDimensions()
{
	Vector2n MinDimensions(3 * charWidth, lineHeight);

	Vector2n MaxDimensions(MinDimensions);
	for (auto & Entry : m_List)
	{
		MaxDimensions.X() = std::max<sint32>(MaxDimensions.X(), Concept::GetDimensions(Entry).X());
	}
	MaxDimensions.Y() = std::max<sint32>(MaxDimensions.Y(), static_cast<sint32>(m_List.size() + 0) * lineHeight);

	// TEST
	if (!m_TypingModule.GetString().empty())
	{
		MaxDimensions.Y() = std::max<sint32>(MaxDimensions.Y(), static_cast<sint32>(m_List.size() + 1) * lineHeight);

		for (auto & Pointer : GetGestureRecognizer().GetConnected())
		{
			if (Pointer::VirtualCategory::POINTING == Pointer->GetVirtualCategory())
			{
				MaxDimensions.X() = std::max<sint32>(MaxDimensions.X(), static_cast<sint32>(m_TypingModule.GetString().length() * charWidth));
			}
		}
	}

	Vector2n Dimensions = MaxDimensions;

	SetDimensions(Dimensions);
}

template <typename T> void ListWidget<T>::ProcessManipulationStarted(const PointerState & PointerState)
{
	if (!HasTypingFocus())
	{
		auto ParentLocalPosition = GlobalToParent(Vector2n(PointerState.GetAxisState(0).GetPosition(), PointerState.GetAxisState(1).GetPosition()));
		
		ModifyGestureRecognizer().m_ManipulationOffset = GetPosition() - ParentLocalPosition;
	}
}

template <typename T> void ListWidget<T>::ProcessManipulationUpdated(const PointerState & PointerState)
{
	if (!HasTypingFocus())
	{
		auto ParentLocalPosition = GlobalToParent(Vector2n(PointerState.GetAxisState(0).GetPosition(), PointerState.GetAxisState(1).GetPosition()));
		
		ModifyPosition() = GetGestureRecognizer().m_ManipulationOffset + ParentLocalPosition;
	}
}

template <typename T> void ListWidget<T>::ProcessManipulationCompleted(const PointerState & PointerState)
{
}