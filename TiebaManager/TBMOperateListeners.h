#pragma once
class CTBMOperate;
class CEventBase;


class CTBMOperateListeners
{
protected:
	CTBMOperate& m_operate;

public:
	CTBMOperateListeners(CTBMOperate& operate);

	void OnComfirm(CEventBase* event__);

	void OnPostBan(CEventBase* event__);
};
