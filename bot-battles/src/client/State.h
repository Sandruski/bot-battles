#ifndef __STATE_H__
#define __STATE_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class State
	{
	public:
		State() {}
		virtual ~State() {}

		virtual bool Create() = 0;
		virtual bool Destroy() = 0;

		virtual bool Enter() { return true; }
		virtual bool PreUpdate(F32 /*dt*/) { return true; }
		virtual bool Update(F32 /*dt*/) { return true; }
		virtual bool PostUpdate(F32 /*dt*/) { return true; }
		virtual bool Render() { return true; }
		virtual bool Exit() { return true; }
	};
}

#endif
