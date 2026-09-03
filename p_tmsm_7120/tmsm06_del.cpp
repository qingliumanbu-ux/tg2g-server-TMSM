/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 
日期: 2014-05-07
功能: 钢包热修实绩删除
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm04.h"

/*<remark>=========================================================
/// <summary>
/// 钢包热修实绩删除
/// <para>
/// 1.接收传入的钢包热修实绩删除信息；
/// 2.删除钢包热修实绩删除信息；
/// </para>
/// <para>数据库表：TTMSM04(钢包热修实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面使用实绩删除调用；TTMSM06画面F5(删除)调用。   </para>
/// </summary>
/// <param name="bcls_rec">删除的钢包热修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm06_del)

int f_tmsm06_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM04 ttmsm04(conn);
	
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
			ttmsm04.Reset();
			ttmsm04.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			Log::Trace("","tmsm09_upd","ttmsm04.USE_SEQ_NO [{0}]"	,ttmsm04.USE_SEQ_NO);
			Log::Trace("","tmsm09_upd","ttmsm04.LADLE_NO [{0}]"		,(const char*)ttmsm04.LADLE_NO);
			Log::Trace("","tmsm09_upd","ttmsm04.SM_UNIT_NO [{0}]"	,(const char*)ttmsm04.SM_UNIT_NO);

			// 判传入的参数是否有错	
			if(ttmsm04.USE_SEQ_NO <= 0)
			{
				sprintf	(s.msg , "传入的使用流水号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			if(ttmsm04.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			if(ttmsm04.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			
			/*查询钢包使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
				     "  FROM TTMSM04 "
					 "  WHERE USE_SEQ_NO	= @ttmsm04.USE_SEQ_NO "
					 "  AND SM_UNIT_NO		= @ttmsm04.SM_UNIT_NO "
					 "  AND LADLE_NO		= @ttmsm04.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm04.USE_SEQ_NO"	, ttmsm04.USE_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm04.LADLE_NO"		, ttmsm04.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm04.SM_UNIT_NO"	, ttmsm04.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			Log::Trace("","tmsm06_del","count [{0}]",count );
			cmd_inq.Close();
			if	(count == 0)
			{
				sprintf	(s.msg ,"钢包使用实绩信息不存在");
				/*新增钢包使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm06_del");
			}
			
			//根据传入信息删除钢包使用实绩表
			sqlstr = "DELETE TTMSM04";
			ttmsm04.Delete(" LADLE_NO ");
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