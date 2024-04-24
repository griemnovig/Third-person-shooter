#pragma once

class AnimUtils
{
public:
	
	template<typename T>
	static T* FindFirstNotifyByClass(UAnimSequenceBase* Animation)
	{
		if (!Animation) return nullptr;
		
		const auto NotifyEvents = Animation->Notifies;
		for (auto NotifyEvent : NotifyEvents)
		{
			auto AnimNotifyState = Cast<T>(NotifyEvent.NotifyStateClass.Get());
			if (AnimNotifyState)
			{
				return AnimNotifyState;
			}
		}
		return nullptr;
	}
};
