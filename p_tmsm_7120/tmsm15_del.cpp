/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-12
功能: 铁水罐维修实绩删除
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm15.h"

/*<remark>=========================================================
/// <summary>
/// 铁水罐维修实绩删除
/// <para>
/// 1.接收传入的铁水罐维修实绩信息；
/// 2.删除铁水罐维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM15(铁水罐维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM11画面维修实绩删除调用；TTMSM15画面F5(删除)调用。   </para>
/// </summary>
/// <param name="bcls_rec">删除的铁水罐维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm15_del)

int f_tmsm15_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM15 ttmsm15(conn);
	
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
			ttmsm15.Reset();
			ttmsm15.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			Log::Trace("","tmsm15_del","ttmsm15.REPAIR_SEQ_NO [{0}]",ttmsm15.REPAIR_SEQ_NO);
			Log::Trace("","tmsm15_del","ttmsm15.IRON_LADLE_NO [{0}]",(const char*)ttmsm15.IRON_LADLE_NO);
			Log::Trace("","tmsm15_del","ttmsm15.SM_UNIT_NO [{0}]"	,(const char*)ttmsm15.SM_UNIT_NO);
			
			// 判传入的参数是否有错	
			if(ttmsm15.REPAIR_SEQ_NO <= 0)
			{
				sprintf	(s.msg , "传入的维修流水号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm15_del");
			}
			if(ttmsm15.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm15_del");
			}
			if(ttmsm15.IRON_LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的铁水罐号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm15_del");
			}
			
			/*查询铁水罐维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
				     "  FROM TTMSM15 "
					 "  WHERE REPAIR_SEQ_NO	= @ttmsm15.REPAIR_SEQ_NO "
					 "  AND SM_UNIT_NO		= @ttmsm15.SM_UNIT_NO "
					 "  AND IRON_LADLE_NO	= @ttmsm15.IRON_LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm15.REPAIR_SEQ_NO"	, ttmsm15.REPAIR_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm15.IRON_LADLE_NO"	, ttmsm15.IRON_LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm15.SM_UNIT_NO"	, ttmsm15.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			Log::Trace("","tmsm15_del","count [{0}]",count );
			cmd_inq.Close();
			if	(count == 0)
			{
				sprintf	(s.msg ,"铁水罐维修实绩信息不存在");
				/*新增铁水罐维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm15_del");
			}
			
			//根据传入信息删除铁水罐维修实绩表
			sqlstr = "DELETE TTMSM15";
			ttmsm15.Delete(" REPAIR_SEQ_NO,SM_UNIT_NO,IRON_LADLE_NO ");
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