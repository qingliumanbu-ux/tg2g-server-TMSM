/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-02
Description: 连铸机设备周期管理扇形段记录导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm17av_f3)

int f_tmsm17av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	CModel ttmsm17("TTMSM17");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm17.Reset();
			ttmsm17.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm17["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm17["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm17["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm17["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			//ttmsm17["DATE_C"]=datetime.SubstringNE(0,8);		//填表日期
			//数据校验等操作可在此进行
			if (ttmsm17["DATE_C"].ToString() == " ")
			{
				strcpy(s.msg, "日期为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm17["CCMACH_DESC"].ToString() == " ")
			{
				strcpy(s.msg, "铸机为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm17["SEQ_NO"].ToString() == " ")
			{
				strcpy(s.msg, "序号为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}

			sqlstr = "SELECT COUNT(*) FROM TTMSM17 WHERE CCMACH_DESC = @CCMACH_DESC "
				" AND  DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO "
				;
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("CCMACH_DESC", ttmsm17["CCMACH_DESC"].ToString());
			cmd1.Parameters.Set("DATE_C", ttmsm17["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm17["SEQ_NO"].ToString());
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();

			if (con>1)
			{
				strcpy(s.msg, "数据已存在,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			ttmsm17.Insert();
		}

	}
	catch (const CApplicationException& ex)

	{

		doFlag = ex.GetCode();

		strcpy(s.msg, (const char*)ex.GetMsg());

	}

	catch (CException& ex)  //用于捕获数据库操作异常

	{

		strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EIManager.Instance.CallService(v_curr_part_name,)方法返回的EI.EIInfo对象的sys_info.msg参数对应

		s.flag = ex.GetCode();       //返回前台，与EI.EIManager.Instance.CallService(v_curr_part_name,)方法返回的EI.EIInfo对象的sys_info.flag参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚

	}

	//返回-1时事务将回滚，返回为0是事务将提交

	return(doFlag);
}


