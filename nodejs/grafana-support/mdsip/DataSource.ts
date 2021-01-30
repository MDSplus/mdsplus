import defaults from 'lodash/defaults';

import {
  DataQueryRequest,
  DataQueryResponse,
  DataSourceApi,
  DataSourceInstanceSettings,
  //MutableDataFrame,
  FieldType,
} from '@grafana/data';

import { Observable, merge } from 'rxjs';
import { MyQuery, MyDataSourceOptions, defaultQuery } from './types';
import { CircularDataFrame } from '@grafana/data';
import * as mdsipFunctions from './mdsip-client';

export class DataSource extends DataSourceApi<MyQuery, MyDataSourceOptions> {
  ipAddress: string;
  port: number;
  connectionId: number;
  constructor(instanceSettings: DataSourceInstanceSettings<MyDataSourceOptions>) {
    super(instanceSettings);
    this.ipAddress = instanceSettings.jsonData.ipAddress || 'localhost';
    this.port = instanceSettings.jsonData.port || 8020;
    this.connectionId = -1;
  }

  query(options: DataQueryRequest<MyQuery>): Observable<DataQueryResponse> {
    const { range } = options;
    const from = range!.from.valueOf();
    const to = range!.to.valueOf();
    //const duration = to - from;
    const streams = options.targets.map(target => {
      const query = defaults(target, defaultQuery);
      return new Observable<DataQueryResponse>(subscriber => {
        const frame = new CircularDataFrame({
          append: 'tail',
          capacity: 20000,
        });

        frame.refId = query.refId;
        //        frame.addField({ name: 'time', type: FieldType.time });
        //        frame.addField({ name: 'value', type: FieldType.number });
        var label: string;
        if (query.signal !== undefined) {
          label = query.signal;
        } else {
          label = 'value';
        }
        frame.addField({ name: 'time', type: FieldType.time });
        frame.addField({ name: label, type: FieldType.number });

        var samples: number[] = [];
        var times: number[] = [];
        mdsipFunctions
          .sendMdsipReq(query.experiment || '', query.shot || 0, query.signal || '', from, to, this.connectionId)
          .then(
            retObj => {
              samples = retObj[0];
              times = retObj[1];
              var len = samples.length;
              if (times.length < len) {
                len = times.length;
              }
              //const step = duration / len;
              for (
                let i = 0;
                i < len;
                i++ //frame.add({time: from + i*step, value: samples[i]});
              ) {
                var frameContent: { [id: string]: number } = {};
                frameContent['time'] = times[i];
                frameContent[label] = samples[i];
                frame.add(frameContent);
                //frame.add({ time: times[i], value: samples[i] });
              }
              subscriber.next({
                data: [frame],
                key: query.refId,
              });
            },
            errorMsg => {
              console.log(errorMsg);
            }
          );

        return () => {};
      });
    });
    return merge(...streams);
  }

  async testDatasource() {
    // Implement a health check for your data source.
    var currStatus = 'success';
    var currMessage = 'Success';
    await mdsipFunctions.openMdsip(this.ipAddress, this.port).then(
      res => {
        this.connectionId = res;
      },
      errorString => {
        currStatus = 'failure';
        currMessage = errorString;
      }
    );
    return {
      status: currStatus,
      message: currMessage,
    };
  }
}
