/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description:中间包信息新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 中间包信息新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明
int f_tmsm_trace97(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);



BM2F_ENTERACE(tmsm21bf3_ins)                                         

int f_tmsm21bf3_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	
	/* 实体类定义 */ 
	CModel ttmsm21("TTMSM21");
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
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "MOLD_TYPE");
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "MOLD_NO");
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			//bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING, "STATUS");
		}
		bcls_rec->Tables["TM0099"].Rows.Clear();
		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm21.Reset();
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm21.TrimOrBlank();
			ttmsm21["SM_UNIT_NO"]      = "A";
			ttmsm21["TD_STATUS"] = "4";
			Log::Trace("",__FUNCTION__,"ttmsm21.SM_UNIT_NO		= [{0}]",(const char*)ttmsm21["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm21.TD_NO	= [{0}]", (const char*)ttmsm21["TD_NO"].ToString());

			/* 检查输入参数合法性 */
			if (ttmsm21["TD_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"中间包号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该钢包号是否存在 */
			if(ttmsm21.QueryCount("SM_UNIT_NO,TD_NO") > 0)
			{
				sprintf(s.msg,"中间包号[%s]已存在",(const char*)ttmsm21["TD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 设置钢包信息表初始值 */
			//ttmsm21["LADLE_LIFE"]		= 0;			//包龄
			//ttmsm21.MAT_LINE_TYPE		= "BW";			//产线类型
			//ttmsm21.MAT_KIND			= "SM";			//物料种类

	 
			ttmsm21["REC_CREATOR"]			= s.userid;		//记录创建责任者
			ttmsm21["REC_CREATE_TIME"]		= datetime;		//记录创建时刻
			ttmsm21.TrimOrBlank();
			ttmsm21.Insert();

			//记录钢包履历
			//bcls_rec->Tables["TM0099"].Rows.Clear();
			ttmsm97.CopyFrom(ttmsm21);
			ttmsm97["MOLD_TYPE"] = "3";
			ttmsm97["MOLD_NO"] = ttmsm21["TD_NO"];
			ttmsm97["EVENT_ID"] = "TM03";					// 事件号		
			ttmsm97["EVENT_DESC"] = "中间包新增";
			ttmsm97["FUNC_ID"] = s.svc_name;											
			ttmsm97["STATUS"] = ttmsm21["TD_STATUS"];			// 产线类型								
			ttmsm97.MergeTo(bcls_rec->Tables["TM0099"], false);
			//bcls_rec->Tables["TM0099"].Rows.Add();
			//bcls_rec->Tables["TM0099"].Rows[i]["MOLD_TYPE"] = "3"; 
			//bcls_rec->Tables["TM0099"].Rows[i]["MOLD_NO"] = ttmsm21["TD_NO"];
			//bcls_rec->Tables["TM0099"].Rows[i]["EVENT_ID"] = "TM01";
			//bcls_rec->Tables["TM0099"].Rows[i]["EVENT_DESC"] = "中间包新增";
			//bcls_rec->Tables["TM0099"].Rows[i]["FUNC_ID"] = s.svc_name; 
			//bcls_rec->Tables["TM0099"].Rows[i]["STATUS"] = ttmsm21["TD_STATUS"];
			
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

