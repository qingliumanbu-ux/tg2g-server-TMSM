/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-15
Description: 连铸机扇形段标定数据导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm16av_f3)

int f_tmsm16av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	CModel ttmsm16("TTMSM16");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{
		ttmsm16.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (ttmsm16.QueryCount("CC_MACH_NO,STRAND_DIV") > 0) {
			if (ttmsm16.QueryCount("CC_MACH_NO,STRAND_DIV,DATE_C") > 0)
			{
				sqlstr = " delete from ttmsm16 where DATE_C = '" + ttmsm16["DATE_C"].ToString() + "' AND CC_MACH_NO = '" + ttmsm16["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm16["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();
			}
			else {
				/*sqlstr = " insert into htmsm15 select * from ttmsm16 where CC_MACH_NO = '" + ttmsm16["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm16["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				sqlstr = " delete from ttmsm16  where CC_MACH_NO = '" + ttmsm16["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm16["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();*/
			}
		}

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm16.Reset();
			ttmsm16.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm16["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm16["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm16["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm16["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			//ttmsm16["DATE_C"] = datetime.SubstringNE(0, 8);		//填表日期
			//数据校验等操作可在此进行
			if (ttmsm16["CC_MACH_NO"].ToString() == " ")
			{
				strcpy(s.msg, "连铸机号为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm16["STRAND_DIV"].ToString() == " ")
			{
				strcpy(s.msg, "流号区分为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm16["SEQ_NO"].ToString() == " ")
			{
				strcpy(s.msg, "序号为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
		
			
			ttmsm16.Insert();
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


