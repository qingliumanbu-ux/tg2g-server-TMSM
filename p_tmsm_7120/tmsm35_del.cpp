/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 结晶器维修实绩删除
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm35.h"

/*<remark>=========================================================
/// <summary>
/// 结晶器维修实绩删除
/// <para>
/// 1.接收传入的结晶器维修实绩信息；
/// 2.删除结晶器维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM35(结晶器维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM31画面维修实绩删除调用；TTMSM32画面F5(删除)调用。   </para>
/// </summary>
/// <param name="bcls_rec">删除的结晶器维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm35_del)

int f_tmsm35_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */

	/* 实体类定义 */
	CTTMSM35 ttmsm35(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{		
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count(); 
		
		//循环处理
		for (int i = 0; i < rows ; i++ )
		{
			ttmsm35.Reset();
			ttmsm35.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			Log::Trace("","tmsm35_del","ttmsm35.REPAIR_SEQ_NO [{0}]",ttmsm35.REPAIR_SEQ_NO);
			Log::Trace("","tmsm35_del","ttmsm35.MOLD_NO [{0}]"		,(const char*)ttmsm35.MOLD_NO);
			Log::Trace("","tmsm35_del","ttmsm35.SM_UNIT_NO [{0}]"	,(const char*)ttmsm35.SM_UNIT_NO);
			Log::Trace("","tmsm35_del","ttmsm35.MOLD_SECTION [{0}]"	,(const char*)ttmsm35.MOLD_SECTION);
			Log::Trace("","tmsm35_del","ttmsm35.CC_MACH_NO [{0}]"	,(const char*)ttmsm35.CC_MACH_NO);
			
			// 判传入的参数是否有错	
			if(ttmsm35.REPAIR_SEQ_NO <= 0)
			{
				sprintf	(s.msg , "传入的维修流水号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			if(ttmsm35.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			if(ttmsm35.MOLD_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			if(ttmsm35.MOLD_SECTION.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器断面不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			if(ttmsm35.CC_MACH_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器所在连铸机号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			
			/*查询结晶器维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
				     "  FROM TTMSM35 "
					 "  WHERE REPAIR_SEQ_NO	= @ttmsm35.REPAIR_SEQ_NO "
					 "  AND SM_UNIT_NO		= @ttmsm35.SM_UNIT_NO "
					 "  AND MOLD_NO			= @ttmsm35.MOLD_NO "
					 "  AND MOLD_SECTION	= @ttmsm35.MOLD_SECTION "
					 "  AND CC_MACH_NO		= @ttmsm35.CC_MACH_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm35.REPAIR_SEQ_NO"	, ttmsm35.REPAIR_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm35.MOLD_NO"		, ttmsm35.MOLD_NO );
			cmd_inq.Parameters.Set( "ttmsm35.SM_UNIT_NO"	, ttmsm35.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm35.MOLD_SECTION"	, ttmsm35.MOLD_SECTION );
			cmd_inq.Parameters.Set( "ttmsm35.CC_MACH_NO"	, ttmsm35.CC_MACH_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			Log::Trace("","tmsm35_del","count {0}",count );
			cmd_inq.Close();
			if	(count == 0)
			{
				sprintf	(s.msg ,"结晶器维修实绩信息不存在");
				/*新增结晶器维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm35_del");
			}
			
			//根据传入信息删除结晶器维修实绩表
			sqlstr = "DELETE TTMSM35";
			ttmsm35.Delete(" REPAIR_SEQ_NO,SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO ");
		}

		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);					
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
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
	
	return doFlag;
}