/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-02
Description: 连铸机设备周期管理结晶器记录导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm18av_f3)

int f_tmsm18av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	CModel ttmsm18("TTMSM18");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm18.Reset();
			ttmsm18.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm18["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm18["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm18["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm18["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			//ttmsm18["DATE_C"] = datetime.SubstringNE(0, 8);		//填表日期
			//数据校验等操作可在此进行
			if (ttmsm18["DATE_C"].ToString() == " ")
			{
				strcpy(s.msg, "日期为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm18["MOLD_NO"].ToString() == " ")
			{
				strcpy(s.msg, "结晶器号为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
		
			sqlstr = "SELECT COUNT(*) FROM TTMSM18 WHERE DATE_C = @DATE_C "
				" AND MOLD_NO = @MOLD_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm18["DATE_C"].ToString());
			cmd1.Parameters.Set("MOLD_NO", ttmsm18["MOLD_NO"].ToString());
			Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString(), bcls_rec->Tables[0].Rows[i]["MOLD_NO"].ToString());
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();

			if (con>1)
			{
				strcpy(s.msg, "数据已存在,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			ttmsm18.Insert();
		}

	}
	/*
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	*/
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


