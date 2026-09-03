/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-15
Description: 连铸机扇形段对弧记录导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm15av_f3)

int f_tmsm15av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	CModel ttmsm15("TTMSM15");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);

	try
	{
		ttmsm15.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (ttmsm15.QueryCount("CC_MACH_NO,STRAND_DIV") > 0) {
			if (ttmsm15.QueryCount("CC_MACH_NO,STRAND_DIV,DATE_C") > 0)
			{
				sqlstr = " delete from ttmsm15 where DATE_C = '" + ttmsm15["DATE_C"].ToString() + "' AND CC_MACH_NO = '" + ttmsm15["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm15["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();
			}
			/*else {
				sqlstr = " insert into htmsm15 select * from ttmsm15 where CC_MACH_NO = '" + ttmsm15["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm15["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				sqlstr = " delete from ttmsm15  where CC_MACH_NO = '" + ttmsm15["CC_MACH_NO"].ToString() + "' AND STRAND_DIV = '" + ttmsm15["STRAND_DIV"].ToString() + "' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();
			}*/
		}
		
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm15.Reset();
			ttmsm15.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm15["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm15["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm15["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm15["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			ttmsm15["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
			//数据校验等操作可在此进行
		
			if (ttmsm15["CC_MACH_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "连铸机号为空,请重新输入！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (ttmsm15["STRAND_DIV"].ToString().Trim() == "")
			{
				sprintf(s.msg, "流号区分号为空,请重新输入！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (ttmsm15["ROLLER_NO_DESC"].ToString().Trim() == "")
			{
				sprintf(s.msg, "辊子号为空,请重新输入！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (ttmsm15["SEQ_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "序号为空,请重新输入！");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			
			if (!ttmsm15.Insert()) {
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//返回-1时事务将回滚，返回为0是事务将提交

	return(doFlag);
}


