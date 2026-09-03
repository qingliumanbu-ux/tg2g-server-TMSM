/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 结晶器信息修改
**************************************************/
//框架头文件
#include "stdafx.h" 
/*<remark>=========================================================
/// <summary>
/// 结晶器信息修改
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明
int f_tmsm_trace97(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(tmsm31bf4_upd)                                                     

int f_tmsm31bf4_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	
	/* 实体类定义 */ 
	CModel ttmsm31("TTMSM31");
	CModel ttmsm97("TTMSM97");
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		blkNum = bcls_rec->Tables.IndexOf("TM0099");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("TM0099");
		}
		bcls_rec->Tables["TM0099"].Rows.Clear();

		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm31.Reset();
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm31.TrimOrBlank();
   
			Log::Trace("",__FUNCTION__,"ttmsm31.SM_UNIT_NO		= [{0}]",(const char*)ttmsm31["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm31.MOLD_NO	= [{0}]", (const char*)ttmsm31["MOLD_NO"].ToString());

			/* 检查输入参数合法性 */
			/*if(ttmsm31["SM_UNIT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"炼钢单元号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if (ttmsm31["MOLD_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"结晶器号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该钢包号是否存在 */
			if(ttmsm31.QueryCount("SM_UNIT_NO,MOLD_NO") <= 0)
			{
				sprintf(s.msg,"结晶器号[%s]不存在!",(const char*)ttmsm31["MOLD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/* 修改钢包信息 */
			ttmsm31["REC_REVISOR"] = s.userid;   //记录创建责任者
			ttmsm31["REC_REVISE_TIME"] = datetime;   //记录创建时刻
			sqlstr = " REC_REVISOR    "
					 ",REC_REVISE_TIME ";
			for (i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
			{
				CString sColName = bcls_rec->Tables[0].Columns[i].get_ColumnName().ToUpper();

				if (ttmsm31.GetFields().Contains(sColName) &&
					sColName != "SM_UNIT_NO" &&
					sColName != "MOLD_NO" &&
					sColName != "MOLD_TYPE" &&
					sColName != "REC_REVISOR" &&
					sColName != "REC_REVISE_TIME")
				{
					sqlstr = sqlstr + "," + sColName;
				}
			}
			Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		
            ttmsm31.Update(sqlstr,"SM_UNIT_NO,MOLD_NO");
			ttmsm31.Query("SM_UNIT_NO,MOLD_NO");
			//记录钢包履历
			//bcls_rec->Tables["TM0099"].Rows.Clear();
			ttmsm97.CopyFrom(ttmsm31);
			ttmsm97["EVENT_ID"] = "TM04";					// 事件号		
			ttmsm97["EVENT_DESC"] = "修改";
			ttmsm97["FUNC_ID"] = s.svc_name;
			ttmsm97["STATUS"] = ttmsm31["CURRENT_STATUS"];			// 产线类型								
			ttmsm97.MergeTo(bcls_rec->Tables["TM0099"], false);
		}
		doFlag = f_tmsm_trace97(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

