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
import { StreamQuery, StreamDataSourceOptions, defaultQuery } from './types';
import { CircularDataFrame } from '@grafana/data';
import * as streamFunctions from './stream-client';

export class DataSourceStream extends DataSourceApi<StreamQuery, StreamDataSourceOptions> {
  port: number;

  constructor(instanceSettings: DataSourceInstanceSettings<StreamDataSourceOptions>) {
    super(instanceSettings);
    this.port = instanceSettings.jsonData.port || 8020;
  }

  query(options: DataQueryRequest<StreamQuery>): Observable<DataQueryResponse> {
    const { range } = options;
    const from = range!.from.valueOf();
    const to = range!.to.valueOf();
    //const duration = to - from;
    const streams = options.targets.map(target => {
      const query = defaults(target, defaultQuery);
      return new Observable<DataQueryResponse>(subscriber => {
        const frame = new CircularDataFrame({
          append: 'tail',
          capacity: 40000,
        });

        frame.refId = query.refId;
        var label: string;
        if (query.label !== undefined) {
          label = query.label;
        } else if (query.signal !== undefined) {
          label = query.signal;
        } else {
          label = 'value';
        }
        frame.addField({ name: 'time', type: FieldType.time });
        frame.addField({ name: label, type: FieldType.number });

        //  var samples: number[] = [];
        //  var times: number[] = [];
        var update = (samples: number[], times: number[]) => {
          var len = samples.length;
          if (times.length < len) {
            len = times.length;
          }
          //const step = duration / len;
          for (let i = 0; i < len; i++) {
            var frameContent: { [id: string]: number } = {};
            frameContent['time'] = times[i];
            frameContent[label] = samples[i];
            frame.add(frameContent);
            //frame.add({ time: times[i], label: samples[i] });
          }
          subscriber.next({
            data: [frame],
            key: query.refId,
          });
        };
        var connId = streamFunctions.registerStream(query.signal || '', from, to, update);

        return () => {
          streamFunctions.unregisterStream(connId);
        };
      });
    });
    return merge(...streams);
  }

  async testDatasource() {
    // Implement a health check for your data source.
    var currStatus = 'success';
    var currMessage = 'Success';
    await streamFunctions.connectToStreams(this.port).then(
      res => {},
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
